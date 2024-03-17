# sqlite3-ngram

[sqlite3-ngram](https://github.com/leiless/sqlite3-ngram)のフォークです。
SQLite3の[FTS5](https://sqlite.org/fts5.html)でbigramが使えるようになってうれしいです。

- CMakeでなくGNU makeでビルドできるように修正。
- [Google Logging Library](https://github.com/google/glog)を使用しないように修正。
- [Protocol Buffers](https://github.com/protocolbuffers/protobuf)を使用しないように修正。
    - [highlight](https://sqlite.org/fts5.html#the_highlight_function)を無効化。

## ビルド

```sh
cd src
make
cp libngram.so 好きな場所♡
```

SQLite3が標準的な場所にない場合は`CPPFLAGS`と`LDFLAGS`を設定します。

```sh
cd src
env CPPFLAGS="-I'SQLite3のヘッダがあるディレクトリ'" \
    LDFLAGS="-L'SQLite3のライブラリがあるディレクトリ'" \
    make
cp libngram.so 好きな場所♡
```

## 使い方

```sql
-- macOSに入っているsqlite3はSQLITE_OMIT_LOAD_EXTENSIONなので注意！
.load libngram.so
create virtual table ft using fts5(entry, title, text, tokenize = "ngram");
create virtual table ft_vocab using fts5vocab(ft, row);
```

