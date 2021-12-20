# xeventbind
A small utility that runs your executable/script when interesting X11 events are fired.

## Compilation
Requires xlib
```
git clone https://github.com/ritave/xeventbind.git
cd xeventbind
make
```

## Usage
```
./xeventbind your_executable
```
When an X event is posted, your executable will be run, remember to make it executable and have shebang line.
