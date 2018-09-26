# Website

`jeffniu.com`

## Features

Has a custom basic request cache manager and template engine, supporting
for loops, if/else constructs, key-value pairs, and including other files.

## Requirements

```bash
sudo apt install libboost-all-dev libssl-dev
```

## Get Current Port

```bash
sudo lsof -n -i :8080 | grep LISTEN
```
