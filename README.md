# How to use simulator

In order to compile simulator you have to follow these steps

## Compile

```
make
```

## Run

Input file has to contain '.' (dot) as a floating point separator, not ',' (comma)

```
./simulator file.txt
```

## Output

Important output

```
Energy consumed from grid: number kWh
Energy consumed from grid without solar and battery: number kWh
```

## Plot graph

You must install gnuplot and then in terminal

```
gnuplot
gnuplot> load "printgraph.ps" 
```

This should plot the graph :) enjoy!





