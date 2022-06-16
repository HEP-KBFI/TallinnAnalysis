#include "TallinnAnalysis/Framework/interface/getSubdirectories.h"

#include <TKey.h>    // TKey
#include <TList.h>   // TList
#include <TObject.h> // TObject

std::vector<const TDirectory *>
getSubdirectories(const TDirectory * dir)
{
  std::vector<const TDirectory *> subdirectories;
  TList * list = dir->GetListOfKeys();
  TIter next(list);
  TKey * key = nullptr;

  while ( (key = dynamic_cast<TKey*>(next())) )
  {
    const TObject * object = key->ReadObj();
    const TDirectory * subdirectory = dynamic_cast<const TDirectory *>(object);
    if ( subdirectory )
    {
      subdirectories.push_back(subdirectory);
    }
    else
    {
      delete object;
    }
  }
  return subdirectories;
}
