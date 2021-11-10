# Histogram Equalization

Histogram equalization capabilities in GIL are demonstrated by the program `histogram_equalization`, compiled from the sources `example/histogram_equalization.cpp`.

## Synopsis
`histogram_equalization`

The program doesn't take any argument on the command line.

`histogram_equalization` expects to find an image called `test_adaptive.png` in the current directory, and produces an image in return, where the equalization have been applied: `histogram_gray_equalized.png`.

## Specific requirements

### Build requirements
- A C++ compiler compliant with C++11 or above
- The PNG library installed and configured.

### Execution requirements
- `histogram_equalization` expects to find an image called `test_adaptive.png` in the current directory.
