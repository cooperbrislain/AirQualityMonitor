#ifndef STREAMPRINT_H
#define STREAMPRINT_H

template<class T> inline Print &operator <<(Print &stream, T arg) { stream.print(arg); return stream; }

#endif // STREAMPRINT_H
