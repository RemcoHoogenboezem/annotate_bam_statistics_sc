//----------------------------------------------------------------
// Name        : bam_file.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description :
//----------------------------------------------------------------
#include <iostream>
#include "bam_file.h"
//----------------------------------------------------------------
int BamFile::PileupCallback(void * userData,bam1_t * alignment)
{
    return reinterpret_cast<BamFile*>(userData)->ReadRegion(alignment);
}
//----------------------------------------------------------------
BamFile::BamFile(void) : initialized(false) , fileHandle(nullptr) , bamHeader(nullptr) , bamIndex(nullptr) ,pileupObject(nullptr) , bamRegion(nullptr)
{
}
//----------------------------------------------------------------
BamFile::~BamFile(void)
{
    Close();
}
//----------------------------------------------------------------
bool BamFile::Open(const string & fileName,uint32_t minQual,bool countDuplicates)
{
    if(this->fileName.compare(fileName)==0) return true;

    bamFile fileHandle=bam_open(fileName.c_str(),"r");

    if(fileHandle==nullptr)
    {
        cerr << "Error: Could not open bam file" << endl;
        return false;
    }

    bam_header_t * bamHeader=bam_header_read(fileHandle);

    if(bamHeader==nullptr)
    {
        bam_close(fileHandle);
        cerr << "Error: Could not read bam header" << endl;
        return false;
    }

    bam_index_t * bamIndex=bam_index_load(fileName.c_str());

    if(bamIndex==nullptr)
    {
        bam_header_destroy(bamHeader);
        bam_close(fileHandle);
        cerr << "Error: Could not read bam index file" << endl;
        return false;
    }

    bam_plp_t pileupObject=bam_plp_init(PileupCallback,reinterpret_cast<void*>(this));

    if(pileupObject==nullptr)
    {
        bam_index_destroy(bamIndex);
        bam_header_destroy(bamHeader);
        bam_close(fileHandle);
        cerr << "Error: Could not create pileup object" << endl;
        return false;
    }

    int mask=BAM_FUNMAP|BAM_FQCFAIL;

    if(countDuplicates==false)
    {
        mask|=BAM_FDUP;
    }

    bam_plp_set_maxcnt(pileupObject,2147483647);
    bam_plp_set_mask(pileupObject,mask);

    Close();

    this->minQual=minQual;
    this->fileName=fileName;
    this->fileHandle=fileHandle;
    this->bamHeader=bamHeader;
    this->bamIndex=bamIndex;
    this->pileupObject=pileupObject;

    initialized=true;

    return true;
}
//----------------------------------------------------------------
void BamFile::Close(void)
{
    initialized=false;

    if(bamRegion)
    {
        bam_iter_destroy(bamRegion);
        bamRegion=nullptr;
    }

    if(pileupObject)
    {
        bam_plp_destroy(pileupObject);
        pileupObject=nullptr;
    }

    if(bamIndex)
    {
        bam_index_destroy(bamIndex);
        bamIndex=nullptr;
    }

    if(bamHeader)
    {
        bam_header_destroy(bamHeader);
        bamHeader=nullptr;
    }

    if(fileHandle)
    {
        bam_close(fileHandle);
        fileHandle=nullptr;
    }

    fileName.clear();
}
//----------------------------------------------------------------
const char * BamFile::GetFileName(void)
{
    return fileName.c_str();
}
//----------------------------------------------------------------
int BamFile::GetTargetCount(void)
{
    if(initialized==false)
    {
        return 0;
    }

    return bamHeader->n_targets;
}
//----------------------------------------------------------------
int BamFile::GetTargetLen(int targetId)
{
    if(initialized==false || targetId<0 || targetId>=bamHeader->n_targets)
    {
        return 0;
    }

    return static_cast<int>(bamHeader->target_len[targetId]);
}
//----------------------------------------------------------------
const char * BamFile::GetTargetName(int targetId)
{
    if(initialized==false || targetId<0 || targetId>=bamHeader->n_targets)
    {
        return nullptr;
    }

    return bamHeader->target_name[targetId];
}
//----------------------------------------------------------------
PLP_DATA * BamFile::GetPileupData(void)
{
    if(initialized==false)
    {
        return nullptr;
    }

    return &pileupData;
}
//----------------------------------------------------------------
bool BamFile::SetRegion(const string & region)
{
    if(initialized==false)
    {
        return false;
    }

    int targetId,begin,end;

    if(bam_parse_region(bamHeader,region.c_str(),&targetId,&begin,&end)==-1)
    {
        return false;
    }

    bam_iter_t bamRegion=bam_iter_query(bamIndex,targetId,begin,end);

    if(bamRegion==nullptr)
    {
        return false;
    }

    bam_plp_reset(pileupObject);

    if(this->bamRegion)
    {
        bam_iter_destroy(this->bamRegion);
    }

    this->bamRegion=bamRegion;

    return true;
}
//----------------------------------------------------------------
bool BamFile::PileupRegion(void)
{
    return (pileupData.alignments=bam_plp_auto(pileupObject,&pileupData.targetId,&pileupData.targetPos,&pileupData.nAlignments))!=nullptr;
}
//----------------------------------------------------------------
int BamFile::Read(bam1_t * alignment)
{
    int ret;

    for(ret=bam_read1(fileHandle,alignment);ret>=0 && (alignment->core.tid<0 || alignment->core.qual<minQual);ret=bam_read1(fileHandle,alignment));

    return ret;
}
//----------------------------------------------------------------
int BamFile::ReadRegion(bam1_t * alignment)
{
    int ret;

    for(ret=bam_iter_read(fileHandle,bamRegion,alignment);ret>=0 && (alignment->core.tid<0 || alignment->core.qual<minQual);ret=bam_iter_read(fileHandle,bamRegion,alignment));

    return ret;
}
//----------------------------------------------------------------
