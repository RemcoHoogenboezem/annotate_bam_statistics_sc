# annotate_bam_statistics_sc

annotate_bam_statistics_sc

A simple tool to annotate annovar files with read statistics from a single cell bam file. The software currently uses Smith and Waterman alignment to perform splice aware local realigment to increase sensitivity. The Smith and Waterman alignment was implemented by Mengyao Zhao (https://github.com/mengyao/Complete-Striped-Smith-Waterman-Library). File IO on bam files was performed using the samtools api. (v0.1.19) 

annotate_bam_statistics_sc [options] > annovarFileOut.txt (output always to std::out)

- -b --bam-file <text>           Single bam file (required)
- -a --annovar-file <text>       Single annovar file (required)
- -B --barcodes-file <text>      Single barcodes file (required)
- -f --fasta-file <text>         Single fasta file (required)
- -s --min-alignment-score <int> Min alignment score (optional default 40)
- -S --min-base-score <int>      Min base score (optional default 30)
- -t --threads <int>             Number of threads (optional default 1)
- -h --help <void>               This help


*Dependencies*
 - gcc 4.3+
 - g++ 4.3+


