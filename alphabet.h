//----------------------------------------------------------------
#ifndef ALPHABET_H
#define ALPHABET_H
//----------------------------------------------------------------
#define A   1
#define C   2
#define G   4
#define T   8
#define N   15
#define INS 16
#define DEL 32
//----------------------------------------------------------------
const char bamSeq2Ascii[]="NACNGNNNTNNNNNNN";
//----------------------------------------------------------------
const int ascii2BamSeq[256]=
{
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,A,N,C,N,N,N,G,N,N,N,N,N,N,N,N,
    N,N,N,N,T,N,N,N,N,N,N,N,N,N,N,N,
    N,A,N,C,N,N,N,G,N,N,N,N,N,N,N,N,
    N,N,N,N,T,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
    N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N
};
//----------------------------------------------------------------
#endif // ALPHABET_H
