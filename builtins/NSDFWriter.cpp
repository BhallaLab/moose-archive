// NSDFWriter.cpp --- 
// 
// Filename: NSDFWriter.cpp
// Description: 
// Author: subha
// Maintainer: 
// Created: Thu Jun 18 23:16:11 2015 (-0400)
// Version: 
// Last-Updated: 
//           By: 
//     Update #: 0
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 
// 
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
// 
// 

// Code:
#ifdef USE_HDF5

#include "hdf5.h"

#include "header.h"
#include "../utility/utility.h"

#include "HDF5WriterBase.h"
#include "HDF5DataWriter.h"

#include "NSDFWriter.h"


const Cinfo * NSDFWriter::initCinfo()
{    
    static FieldElementFinfo< NSDFWriter, InputVariable > eventInputFinfo(
        "eventInput",
        "Sets up field elements for event inputs",
        InputVariable::initCinfo(),
        &NSDFWriter::getEventInput,
        &NSDFWriter::setNumEventInputs,
        &NSDFWriter::getNumEventInputs);
    
    static DestFinfo process(
        "process",
        "Handle process calls. Collects data in buffer and if number of steps"
        " since last write exceeds flushLimit, writes to file.",
        new ProcOpFunc<NSDFWriter>( &NSDFWriter::process));

    static  DestFinfo reinit(
        "reinit",
        "Reinitialize the object. If the current file handle is valid, it tries"
        " to close that and open the file specified in current filename field.",
        new ProcOpFunc<NSDFWriter>( &NSDFWriter::reinit ));

    static Finfo * processShared[] = {
        &process, &reinit
    };
    
    static SharedFinfo proc(
        "proc",
        "Shared message to receive process and reinit",
        processShared, sizeof( processShared ) / sizeof( Finfo* ));

    static Finfo * finfos[] = {
        &eventInputFinfo,
        &proc,
    };

    static string doc[] = {
        "Name", "NSDFWriter",
        "Author", "Subhasis Ray",
        "Description", "NSDF file writer for saving data."
    };

    static Dinfo< NSDFWriter > dinfo;
    static Cinfo cinfo(
        "NSDFWriter",
        HDF5DataWriter::initCinfo(),
        finfos,
        sizeof(finfos)/sizeof(Finfo*),
        &dinfo,
	doc, sizeof( doc ) / sizeof( string ));

    return &cinfo;
}

static const Cinfo * nsdfWriterCinfo = NSDFWriter::initCinfo();

NSDFWriter::NSDFWriter()
{
    cout << "####eventInputs size " << eventInputs_.size() <<endl;
    ;
}

NSDFWriter::~NSDFWriter()
{
    // flush data
}

void NSDFWriter::closeUniformData()
{
    for (unsigned int ii = 0; ii < datasets_.size(); ++ii){
        H5Dclose(datasets_[ii]);
    }
    data_.clear();
    src_.clear();
    func_.clear();
    datasets_.clear();

}

/**
   Handle the datasets for the requested fields (connected to
   requestOut). This is is similar to what HDF5DataWriter does. 
 */
void NSDFWriter::openUniformData(const Eref &eref)
{
    cout << "#### here" << endl;
    const SrcFinfo * requestOut = (SrcFinfo*)eref.element()->cinfo()->findFinfo("requestOut");
    unsigned int numTgt = eref.element()->getMsgTargetAndFunctions(eref.dataIndex(),
                                                                requestOut,
                                                                src_,
                                                                func_);
    assert(numTgt ==  src_.size());
    cout << "#### eventInputs.size=" << eventInputs_.size() <<endl;
    for (unsigned int ii = 0; ii < func_.size(); ++ii){
        string varname = func_[ii];
        cout << "#### ii=" << ii << ", varname" << varname << endl;
        size_t found = varname.find("get");
        if (found == 0){
            varname = varname.substr(3);
            if (varname.length() == 0){
                varname = func_[ii];
            } else {
                varname[0] = tolower(varname[0]);
            }
        }
        assert(varname.length() > 0);
        string path = src_[ii].path() + "/" + varname;
        hid_t datasetId = getDataset(path);
    // TODO : For uniform data : sort the variables by fieldname and
    // source object class then create 2D datasets for each
    // class/field.  map the source objects to dataset/rowindex.
        datasets_.push_back(datasetId);
    }
    data_.resize(src_.size());
}

void NSDFWriter::closeEventData()
{
    for (unsigned int ii = 0; ii < eventDatasets_.size(); ++ii){
        H5Dclose(eventDatasets_[ii]);
    }
    events_.clear();
    eventInputs_.clear();            
    eventDatasets_.clear();
}

void NSDFWriter::openEventData(const Eref &eref)
{
    for (unsigned int ii = 0; ii < eventInputs_.size(); ++ii){
        stringstream path;
        path << eref.objId().path() << "/" << "eventInput[" << ii << "]";
        ObjId inputObj = ObjId(path.str());
        cout << "%%% " << inputObj << endl;
        Element * el = inputObj.element();
        const Finfo * dest = el->cinfo()->findFinfo("input");
        vector < Id > src;
        el->getNeighbors(src, dest);
        assert(src.size() == 1);
        for (unsigned int jj = 0; jj < src.size(); ++jj){
            cout << "$$$$ " << src[jj].path() << endl;
        }
    }
    // // events is similar to regular data in HDF5DataWriter
    // for (unsigned int ii = 0; ii < eventSrc_.size(); ++ii){
    //     string varname = eventFunc_[ii];
    //     size_t found = varname.find("get");
    //     if (found == 0){
    //         varname = varname.substr(3);
    //         if (varname.length() == 0){
    //             varname = eventFunc_[ii];
    //         } else {
    //             // TODO: there is no way we can get back the original
    //             // field-name case. tolower will get the right name in
    //             // most cases as field names start with lower case by
    //             // convention in MOOSE.
    //             varname[0] = tolower(varname[0]);
    //         }
    //     }
    //     assert(varname.length() > 0);
    //     string path = src_[ii].path() + "/" + varname;
    //     hid_t datasetId = getEventDataset(path);
    //     // TODO we shall allow only spike events - getEventDataset
    //     // will map all paths to spikes
    //     eventDatasets_.push_back(datasetId);
    // }
    // events_.resize(eventSrc_.size());
}

herr_t NSDFWriter::writeEnv()
{
    // TODO complete this
    return 0;
}

void NSDFWriter::flush()
{
    HDF5DataWriter::flush();
    // TODO - append all uniform data
    // TODO - append all event data
    // clear the data buffers
    // flush HDF5 nodes.
}

void NSDFWriter::reinit(const Eref& eref, const ProcPtr proc)
{
    // write environment
    // write model
    // write map
    if (filehandle_ >0){
        close();
    }
    // TODO: what to do when reinit is called? Close the existing file
    // and open a new one in append mode? Or keep adding to the
    // current file?
    if (filename_.empty()){
        filename_ = "moose_data.nsdf.h5";
    }
    openFile();
    cout << "#### element:" << eref.element() << " path=" << eref.objId().path() << endl;
    openUniformData(eref);
    openEventData(eref);
    herr_t err = writeEnv();
    steps_ = 0;
}

void NSDFWriter::process(const Eref& eref, ProcPtr proc)
{
    // TODO: send request for data
    // TODO do actual check on flush limits and write stuff
    
}

NSDFWriter& NSDFWriter::operator=( const NSDFWriter& other)
{
	eventInputs_ = other.eventInputs_;
	for ( vector< InputVariable >::iterator 
					i = eventInputs_.begin(); i != eventInputs_.end(); ++i )
			i->setOwner( this );
        for (unsigned int ii = 0; ii < getNumEventInputs(); ++ii){
            events_[ii].clear();
        }
	return *this;
}

void NSDFWriter::setNumEventInputs(unsigned int num)
{
    unsigned int prevSize = eventInputs_.size();
    eventInputs_.resize(num);
    for (unsigned int ii = prevSize; ii < num; ++ii){
        eventInputs_[ii].setOwner(this);
    }
}

unsigned int NSDFWriter::getNumEventInputs() const
{
    return eventInputs_.size();
}

void NSDFWriter::setEnvironment(string key, string value)
{
    env_[key] = value;
}


void NSDFWriter::setInput(unsigned int index, double value)
{
    events_[index].push_back(value);
}

InputVariable* NSDFWriter::getEventInput(unsigned int index)
{
    static InputVariable dummy;
    if (index < eventInputs_.size()){
        return &eventInputs_[index];
    }
    cout << "Warning: NSDFWriter::getEventInput: index: " << index <<
		" is out of range: " << eventInputs_.size() << endl;
   return &dummy;
}

#endif // USE_HDF5

// 
// NSDFWriter.cpp ends here
