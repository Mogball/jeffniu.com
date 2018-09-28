# Website

`jeffniu.com`

## Features

Has a custom basic request cache manager and template engine, supporting
for loops, if/else constructs, key-value pairs, and including other files.

## Requirements

```bash
sudo apt install libboost-all-dev libssl-dev
sudo apt install clojure1.6

npm install postcss-cli --global
npm install html-minifier --global
```

## Get Current Port

```bash
sudo lsof -n -i :8080 | grep LISTEN
```
