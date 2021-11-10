#include <cstring>
#include <glog/logging.h>

#include "highlight.h"
#include "utils.h"

SQLITE_EXTENSION_INIT3

// see:
//  https://github.com/sqlite/sqlite/blob/master/ext/fts5/fts5_aux.c
//      cad760d16ed403a065dbc90dd5c50f1eb29f5988
//  https://github.com/wangfenjin/simple/blob/master/src/simple_highlight.cc#L84

typedef struct CInstIter CInstIter;
struct CInstIter {
    const Fts5ExtensionApi *pApi;   /* API offered by current FTS version */
    Fts5Context *pFts;              /* First arg to pass to pApi functions */
    int iCol;                       /* Column to search */
    int iInst;                      /* Next phrase instance index */
    int nInst;                      /* Total number of phrase instances */

    /* Output variables */
    int iStart;                     /* First token in coalesced phrase instance */
    int iEnd;                       /* Last token in coalesced phrase instance */
};

/*
** Advance the iterator to the next coalesced phrase instance. Return
** an SQLite error code if an error occurs, or SQLITE_OK otherwise.
*/
static inline int fts5CInstIterNext(CInstIter *pIter) {
    int rc = SQLITE_OK;
    pIter->iStart = -1;
    pIter->iEnd = -1;

    while (rc == SQLITE_OK && pIter->iInst < pIter->nInst) {
        int ip;
        int ic;
        int io; // Token offset
        rc = pIter->pApi->xInst(pIter->pFts, pIter->iInst, &ip, &ic, &io);
        if (rc == SQLITE_OK) {
            if (ic == pIter->iCol) {
                // iEnd is inclusive
                int iEnd = io + pIter->pApi->xPhraseSize(pIter->pFts, ip) - 1;

                DLOG(INFO) << "iPhrase: " << ip << " iCol: " << ic << " iOff: " << io << " iEnd: " << iEnd
                           << " iter.iStart: " << pIter->iStart << " iter.iEnd: " << pIter->iEnd;

                if (pIter->iStart < 0) {
                    pIter->iStart = io;
                    pIter->iEnd = iEnd;
                } else if (io <= pIter->iEnd) {
                    if (iEnd > pIter->iEnd) pIter->iEnd = iEnd;
                } else {
                    break;
                }
            }
            pIter->iInst++;
        }
    }

    if (pIter->iStart >= 0 || pIter->iEnd >= 0) {
        CHECK(pIter->iStart >= 0 && pIter->iEnd >= 0);
        DLOG(INFO) << "Output iStart: " << pIter->iStart << " iEnd: " << pIter->iEnd;
    }

    return rc;
}

/*
** Initialize the iterator object indicated by the final parameter to
** iterate through coalesced phrase instances in column iCol.
*/
static inline int fts5CInstIterInit(
        const Fts5ExtensionApi *pApi,
        Fts5Context *pFts,
        int iCol,
        CInstIter *pIter
) {
    int rc;

    memset(pIter, 0, sizeof(CInstIter));
    pIter->pApi = pApi;
    pIter->pFts = pFts;
    pIter->iCol = iCol;
    rc = pApi->xInstCount(pFts, &pIter->nInst);

    if (rc == SQLITE_OK) {
        rc = fts5CInstIterNext(pIter);
    }

    return rc;
}

typedef struct HighlightContext HighlightContext;
struct HighlightContext {
    CInstIter iter;     /* Coalesced Instance Iterator */
    int iPhrase;        /* Current token offset in zIn[] */
#if 0
    int iRangeStart;    /* First token to include */
    int iRangeEnd;      /* If non-zero, last token to include */
#endif
    const char *zOpen;  /* Opening highlight */
    const char *zClose; /* Closing highlight */
    const char *zIn;    /* Input text */
    int nIn;            /* Size of input text in bytes */
    int iOff;           /* Current offset within zIn[] */
    char *zOut;         /* Output value */
};

/*
** Tokenizer callback used by implementation of highlight() function.
*/
static inline int fts5HighlightCb(
        void *pContext,                 /* Pointer to HighlightContext object */
        int tflags,                     /* Mask of FTS5_TOKEN_* flags */
        const char *pToken,             /* Buffer containing token */
        int nToken,                     /* Size of token in bytes */
        int iStartOff,                  /* Start offset of token */
        int iEndOff                     /* End offset of token */
) {
    if (tflags & FTS5_TOKEN_COLOCATED) return SQLITE_OK;

    UNUSED(pToken, nToken);
    auto p = (HighlightContext *) pContext;
    // Current token offset(index)
    int iPhrase = p->iPhrase++;

    if (iPhrase == p->iter.iStart) {
        DLOG(INFO) << "iStart: " << iStartOff << " " << iEndOff;
    }

    int rc = SQLITE_OK;

    if (iPhrase == p->iter.iEnd) {
        if (iPhrase != p->iter.iStart) {
            DLOG(INFO) << "iEnd: " << iStartOff << " " << iEndOff;
        }

        rc = fts5CInstIterNext(&p->iter);
    }

    return rc;
}

void ngram_highlight(
        const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
        Fts5Context *pFts,              /* First arg to pass to pApi functions */
        sqlite3_context *pCtx,          /* Context for returning result/error */
        int nVal,                       /* Number of values in apVal[] array */
        sqlite3_value **apVal           /* Array of trailing arguments */
) {
    if (nVal != 3) {
        const char *zErr = "wrong number of arguments to function " LIBNAME "_highlight()";
        sqlite3_result_error(pCtx, zErr, -1);
        return;
    }

    HighlightContext ctx;
    memset(&ctx, 0, sizeof(ctx));

    int iCol = sqlite3_value_int(apVal[0]);
    ctx.zOpen = (const char *) sqlite3_value_text(apVal[1]);
    ctx.zClose = (const char *) sqlite3_value_text(apVal[2]);

    DLOG(INFO) << "iCol: " << iCol << " zOpen: " << ctx.zOpen << " zClose: " << ctx.zClose;

    // TODO: do we need to release zIn upon return?
    int rc = pApi->xColumnText(pFts, iCol, &ctx.zIn, &ctx.nIn);
    if (rc == SQLITE_OK && ctx.zIn != nullptr) {
        // Init the iterator and get the first coalesced phrase
        rc = fts5CInstIterInit(pApi, pFts, iCol, &ctx.iter);

        if (rc == SQLITE_OK) {
            rc = pApi->xTokenize(pFts, ctx.zIn, ctx.nIn, (void *) &ctx, fts5HighlightCb);
        }
    }
}
