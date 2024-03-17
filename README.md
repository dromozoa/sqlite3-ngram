# sqlite3-ngram

[sqlite3-ngram](https://github.com/leiless/sqlite3-ngram)のフォークです。
SQLite3の[FTS5](https://sqlite.org/fts5.html)でbigramが使えるようになってうれしいです。

- CMakeでなくGNU makeでビルドできるように修正。
- [Google Logging Library](https://github.com/google/glog)を使用しないように修正。
- [Protocol Buffers](https://github.com/protocolbuffers/protobuf)を使用しないように修正。
    - [highlight](https://sqlite.org/fts5.html#the_highlight_function)を無効化。

## ビルド

```
cd src
make
cp libngram.so 好きな場所♡
```

SQLite3が標準的な場所にない場合は`CPPFLAGS`と`LDFLAGS`を設定します。

```
cd src
env CPPFLAGS="-I'SQLite3のヘッダがあるディレクトリ'" \
    LDFLAGS="-L'SQLite3のライブラリがあるディレクトリ'" \
    make
cp libngram.so 好きな場所♡
```

## 使い方

```
sqlite> -- macOSに入っているsqlite3はSQLITE_OMIT_LOAD_EXTENSIONなので注意！
sqlite> .load libngram.so
sqlite> create virtual table ft using fts5(text, tokenize = 'ngram');
sqlite> create virtual table ft_vocab using fts5vocab(ft, row);
sqlite> insert into ft values('メロスは激怒した。');
sqlite> insert into ft values('必ず、かの邪智暴虐の王を除かなければならぬと決意した。');
sqlite> insert into ft values('メロスには政治がわからぬ。');
sqlite> insert into ft values('メロスは、村の牧人である。笛を吹き、羊と遊んで暮して来た。');
sqlite> insert into ft values('けれども邪悪に対しては、人一倍に敏感であった。');
```

```
sqlite> -- 良く出てきたbigramを調べる。
sqlite> select * from ft_vocab where cnt > 1 order by cnt desc;
た。|4|4
メロ|3|3
ロス|3|3
けれ|2|2
した|2|2
して|2|2
であ|2|2
は、|2|2
らぬ|2|2
スは|2|2
```

```sql
sqlite> -- ランク付きで検索する。
sqlite> select bm25(ft), text from ft where ft match '"メロス"' order by bm25(ft);
-1.3134328358209e-06|メロスは激怒した。
-1.18120805369128e-06|メロスには政治がわからぬ。
-8.42105263157895e-07|メロスは、村の牧人である。笛を吹き、羊と遊んで暮して来た。
sqlite> select bm25(ft), text from ft where ft match '"邪智暴虐"' order by bm25(ft);
-0.959581949407381|必ず、かの邪智暴虐の王を除かなければならぬと決意した。
sqlite> select bm25(ft), text from ft where ft match '"セリヌンティウス"' order by bm25(ft);
```

