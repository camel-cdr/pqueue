# pqueue
A minimal cli tool to execute shell commands parallel, that allows you to easily get 100% CPU usage across all cores when running simple scripts or programs.

## Examples

### Calculate average execution time
```sh
pqueue -c 100 "time -p ./a.out >/dev/null" 2>&1 | awk '/real/{x+=$2; c++} END{print x/c}'
```

### Bruteforce best hash parameter
Where `./a.out` prints the number of hash collisions and the parameters used.
```sh
pqueue "./a.out" > /tmp/tmp &
watch "sort -n /tmp/tmp | tail"
```

## Install
```sh
sudo make clean install
```
