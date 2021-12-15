cmake_minimum_required(VERSION 3.9)

include_directories(samtools)
link_directories(samtools)

project(annotate_bam_statistics_sc_src LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(annotate_bam_statistics_sc_src main.cpp algorithm.cpp algorithm.h bam_file.cpp bam_file.h fasta_file.cpp fasta_file.h barcodes_file.cpp barcodes_file.h annovar_file.cpp annovar_file.h ssw.c ssw.h ssw_cpp.cpp ssw_cpp.h alphabet.h)

find_package(Threads REQUIRED)
target_link_libraries (annotate_bam_statistics_sc_src PRIVATE libbam.a z Threads::Threads)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(annotate_bam_statistics_sc_src PUBLIC OpenMP::OpenMP_CXX)
endif()
