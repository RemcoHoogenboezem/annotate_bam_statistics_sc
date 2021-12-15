//----------------------------------------------------------------
// Name        : main.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description : annotate bam statistics
//----------------------------------------------------------------
#include <iostream>
#include <getopt.h>
#include "fasta_file.h"
#include "algorithm.h"
//----------------------------------------------------------------
#define BAM_FILE            'b'
#define ANNOVAR_FILE        'a'
#define BARCODES_FILE       'B'
#define FASTA_FILE          'f'
#define MIN_ALIGNMENT_SCORE 's'
#define MIN_BASE_SCORE      'S'
#define THREADS             't'
#define HELP                'h'
#define SHORT_OPTIONS       "b:a:B:f:t:h"
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
struct option longOptions[] =
{
    {"bam-file",required_argument,nullptr,BAM_FILE},
    {"annovar-file",required_argument,nullptr,ANNOVAR_FILE},
    {"barcodes-file",required_argument,nullptr,BARCODES_FILE},
    {"fasta-file",required_argument,nullptr,FASTA_FILE},
    {"min-alignment-score",required_argument,nullptr,MIN_ALIGNMENT_SCORE},
    {"min-base-score",required_argument,nullptr,MIN_BASE_SCORE},
    {"threads",required_argument,nullptr,THREADS},
    {"help",no_argument,nullptr,HELP}
};
//----------------------------------------------------------------
int main(int argc, char** argv)
{
    //----------------------------------------------------------------
    //Get options
    //----------------------------------------------------------------

    bool showHelp=(argc==1);
    int option,optionIndex;
    Algorithm algorithm;

    while((option=getopt_long(argc,argv,SHORT_OPTIONS,longOptions,&optionIndex))>=0)
    {
        switch(option)
        {
            case BAM_FILE:

                algorithm.bamFileName=string(optarg);
                break;

            case ANNOVAR_FILE:

                algorithm.annovarFileName=string(optarg);
                break;

            case BARCODES_FILE:

                algorithm.barcodesFileName=string(optarg);
                break;

            case FASTA_FILE:

                algorithm.fastaFileName=string(optarg);
                break;

            case MIN_ALIGNMENT_SCORE:

                algorithm.minAlignmentScore=atoi(optarg);
                break;

            case MIN_BASE_SCORE:

                algorithm.minBaseScore=atoi(optarg);
                break;

            case THREADS:

                algorithm.nThreads=atoi(optarg);
                break;

            case HELP:

                showHelp=true;
                break;
        }
    }

    //----------------------------------------------------------------
    //Show help
    //----------------------------------------------------------------

    if(showHelp)
    {
        cerr << "annotate_bam_statistics_sc [options] > annovarFileOut.txt (output always to std::out)"     << endl;
        cerr                                                                                                << endl;
        cerr << "-b --bam-file <text>           Single bam file (required)"                                 << endl;
        cerr << "-a --annovar-file <text>       Single annovar file (required)"                             << endl;
        cerr << "-B --barcodes-file <text>      Single barcodes file (required)"                            << endl;
        cerr << "-f --fasta-file <text>         Single fasta file (required)"                               << endl;
        cerr << "-s --min-alignment-score <int> Min alignment score (optional default 40)"                  << endl;
        cerr << "-S --min-base-score <int>      Min base score (optional default 30)"                       << endl;
        cerr << "-t --threads <int>             Number of threads (optional default 1)"                     << endl;
        cerr << "-h --help <void>               This help"                                                  << endl;
        cerr                                                                                                << endl;

        return 0;
    }

    //----------------------------------------------------------------
    //Run
    //----------------------------------------------------------------

    if(algorithm.Run()==false)
    {
        return -1;
    }

    //----------------------------------------------------------------
    //Done
    //----------------------------------------------------------------

    return 0;
}
//----------------------------------------------------------------
