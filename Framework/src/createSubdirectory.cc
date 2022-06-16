#include "TallinnAnalysis/Framework/interface/createSubdirectory.h"

#include <TObjArray.h>  // TObjArray
#include <TObjString.h> // TObjString
#include <TString.h>    // TString

#include <assert.h>     // assert()
#include <iostream>     // std::cout

TDirectory *
createSubdirectory(TDirectory * dir,
                   const std::string & subdirName,
                   int verbosity)
{
  if(verbosity)
  {
    std::cout << "<createSubdirectory>:\n"
                 " dir = " << dir << ": name = '" << dir->GetName() << "'\n"
                 " subdirName = '" << subdirName << "'\n";
  }

  dir->cd();
  if(! dir->Get(subdirName.data()))
  {
    if(verbosity)
    {
      std::cout << "--> creating subdir = '" << subdirName << "'\n";
    }
    dir->mkdir(subdirName.data());
  }
  else
  {
    if(verbosity)
    {
      std::cout << "--> subdir = '" << subdirName << "' already exists --> skipping\n";
    }
  }
  TDirectory * subdir = dynamic_cast<TDirectory *>(dir->Get(subdirName.data()));
  assert(subdir);
  return subdir;
}

TDirectory *
createSubdirectory_recursively(TFileDirectory & dir,
                               const std::string & fullSubdirName,
                               int verbosity)
{
  if(verbosity)
  {
    std::cout << "<createSubdirectory_recursively>:\n"
                 " dir = " << &dir << ": name = '" << dir.getBareDirectory()->GetName() << "'\n"
                 " fullSubdirName = '" << fullSubdirName << "'\n";
  }

  TString fullSubdirName_tstring = fullSubdirName.data();
  TObjArray * subdirNames = fullSubdirName_tstring.Tokenize("/");
  int numSubdirectories = subdirNames->GetEntries();

  TDirectory * parent = dir.getBareDirectory();
  for(int idxSubdirectory = 0; idxSubdirectory < numSubdirectories; ++idxSubdirectory)
  {
    const TObjString * subdirName = dynamic_cast<TObjString *>(subdirNames->At(idxSubdirectory));
    assert(subdirName);

    TDirectory * subdir = createSubdirectory(parent, subdirName->GetString().Data(), verbosity);
    parent = subdir;
  }
  delete subdirNames;
  return parent;
}




