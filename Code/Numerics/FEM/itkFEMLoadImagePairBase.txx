/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit (ITK)
  Module:    itkFEMLoadImagePairBase.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

=========================================================================*/
#ifndef _itkFEMLoadImagePairBase_txx_
#define _itkFEMLoadImagePairBase_txx_

#include "itkFEMLoadImagePairBase.h"


namespace itk {
namespace fem {


template<class TReference,class TTarget>
LoadImagePairBase<TReference , TTarget>::LoadImagePairBase()
{
  
  for (int i=0; i<ImageDimension; i++)
  {
    m_RefRadius[i] = 1;
    m_TarRadius[i] = 1;
  }

};

template<class TReference,class TTarget>
void LoadImagePairBase<TReference , TTarget>::SetReferenceImage(ReferenceType* R ) 
{ 
  m_RefImage = R; 
// GET DATA SIZE  BUG!! FIXME!! MUST BE BETTER WAY TO GET SIZE
  RefRegionIteratorType Iter (m_RefImage,m_RefImage->GetLargestPossibleRegion() );
  Iter.GoToEnd();
  typename ReferenceType::IndexType Ind = Iter.GetIndex();    
  for (int i=0; i< ImageDimension; i++) m_RefSize[i]=Ind[i]+1;
};

template<class TReference,class TTarget>
void LoadImagePairBase<TReference , TTarget>::SetTargetImage(TargetType* T ) 
{    
  m_TarImage=T; 
// GET DATA SIZE  BUG!! FIXME!! MUST BE BETTER WAY TO GET SIZE
  TarRegionIteratorType Iter (m_TarImage,m_TarImage->GetLargestPossibleRegion() );
  Iter.GoToEnd();
  typename ReferenceType::IndexType Ind = Iter.GetIndex();   
  for (int i=0; i< ImageDimension; i++) m_TarSize[i]=Ind[i]+1; 
};



} // end namespace fem
} // end namespace itk

#endif
