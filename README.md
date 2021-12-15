# annotate_bam_statistics_sc

annotate_bam_statistics_sc

A simple tool to annotate annovar files with read statistics from a single cell bam file. The software currently uses the smith and waterman alignment implementation by Mengyao Zhao. For reading bam files the samtools api was used. Op

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


