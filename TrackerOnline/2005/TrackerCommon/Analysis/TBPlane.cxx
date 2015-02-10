#include "TBPlane.h"
ClassImp(TBTrack)
ClassImp(TBPlane)
ClassImp(TBHit)

Float_t TBPlane::GetXPos(Int_t i)
{
  TBHit* hx = (TBHit*) fHits->At(i);
  if (hx->fVclu[0] !=0 )
    {
      Float_t x = hx->fXclu[0];
      if (fReadoutx <0) x = (fNstripx-x);
      x *=fPitchx;
      x += fX;
      return x;
    }
  else
    return -999.;

}
Float_t TBPlane::GetYPos(Int_t i)
{
      TBHit* hx = (TBHit*) fHits->At(i);
      if (hx->fVclu[1] !=0 )
        if (fType !=1)
          {
            
            Float_t x = hx->fXclu[1];
            if (fReadouty <0) x = (fNstripy-x);
            x *=fPitchy;
            x += fY;
            return x;
          }
        else
          {
            Float_t x = (hx->fXclu[1]-(64-hx->fXclu[0]/2-6))*0.78;
            x+= fY;
            return x;
          }

      else
        return -999.;

}

void TBTrack::Fit()
{
  Int_t i,j;
  // X fit
  Float_t s11x=0;
  Float_t s12x=0;
  Float_t s22x=0;
  Float_t g1x =0;
  Float_t g2x=0;
  Float_t s11y=0;
  Float_t s12y=0;
  Float_t s22y=0;
  Float_t g1y =0;
  Float_t g2y=0;
  Float_t Dy=0;
  Float_t Dx=0;
  Float_t x,y,z,dx,dy;
  Int_t nplane=0;
  Int_t nxinfo=0;
  Int_t nyinfo=0;
  fndfx = 0;
  fndfy = 0;
  memset(fCov,0,10*sizeof(Float_t));
  for ( i=0;i<fPlanes->GetEntriesFast();i++) 
    { 
      TBPlane* p = (TBPlane*) fPlanes->At(i); 
      nplane++;
      x=p->GetXPos(fHitidx[i]);
      dx=p->GetDXPos(fHitidx[i]);
      y=p->GetYPos(fHitidx[i]);
      dy=p->GetDYPos(fHitidx[i]);

      z=p->fZ;
      //      cout << x << " " << dx << endl;
      //      prInt_tf (" %x %g %g %g %g \n",h,x,y,dx,dy);
      // prInt_tf (" %x %g %g %g %g \n",h,h->GetX(),h->GetY(),h->GetDX(),h->GetDY());
      if (x!=0 && x > -999 ) 
        {    
          //          cout << dx << endl;
	  nxinfo++;
          s11x += 1.0/dx/dx;
          g1x += x/dx/dx;
          g2x += x*z/dx/dx;
          s12x += z/dx/dx;
          s22x += z*z/dx/dx;
        }
      if (y!=0 && y > -999 ) 
        {          
	  //          prInt_tf (" dy  %g %g \n",y,dy);
	  nyinfo++;
          s11y += 1.0/dy/dy;
          g1y += y/dy/dy;
          g2y += y*z/dy/dy;
          s12y += z/dy/dy;
          s22y += z*z/dy/dy;
        }
    }

  Dx=s11x*s22x-s12x*s12x;
  if (nxinfo>1) 
    {
      fbx = (g1x/Dx)*s22x;
      fbx -=(g2x/Dx)*s12x;
      fax = (g2x/Dx)*s11x;
      fax -=(g1x/Dx)*s12x;
      fCov[0] = s22x/Dx;
      fCov[1] = -s12x/Dx;
      fCov[2] = s11x/Dx;

      fndfx+=nxinfo-2;
    }

  Dy = s11y*s22y-s12y*s12y;
  if (nyinfo>1) 
    {
      fby = (g1y*s22y-g2y*s12y)/Dy;
      fay = (g2y*s11y-g1y*s12y)/Dy;
      fCov[5] = s22y/Dy;
      fCov[8] = -s12y/Dy;
      fCov[9] = s11y/Dy;
      fndfy+=nyinfo-2;
    }
  //
  fndf =fndfx+fndfy;
  if (fndf==0) fndf++;
  fchi2x = 0;
  fchi2y = 0;
  fchi2 = 0;
  //  prInt_tf(" Dx %g Dy %g \n",Dx,Dy);
  if (Dx == 0 && Dy == 0) return;
  if (nplane>2)
    {
      //  prInt_tf ("Track a = %f b = %f %f %g %g \n",a,b,fCov[0],fCov[1],fCov[2]);
      // prInt_tf ("Track nplane = %d dx = %g %g %g %g %g %g \n",nplane,Dx,s11x,s22x,s12x,g1x,g2x);
    }
  // Chi2
  

  for (  i=0;i<fPlanes->GetEntriesFast();i++) 
    { 
      TBPlane* p = (TBPlane*) fPlanes->At(i); 
      x=p->GetXPos(fHitidx[i]);
      dx=p->GetDXPos(fHitidx[i]);
      y=p->GetYPos(fHitidx[i]);
      dy=p->GetDYPos(fHitidx[i]);

      z=p->fZ;

      Double_t xext = fbx+z*fax;
      // Double_t dxext2 = fCov[0]+2*z*fCov[1]+fCov[2]*z*z;
      if (x>0 && x > -999 ) 
        fchi2x += (x-xext)*(x-xext)/dx/dx;
        
      
      Double_t yext = fby+z*fay;
      //      Double_t dyext2 = fCov[5]+2*z*fCov[8]+fCov[9]*z*z;
    if (y>0 && y > -999) 
      fchi2y += (y-yext)*(y-yext)/dy/dy;
    //  if (fHits.length()>=4)
    //prInt_tf ("%d chi2 = %f %f \n \t %f %f %f \n\n \t %f %f %f \n",i,fChi2,z,
    //	      x,xext,h->GetDX(),y,yext,h->GetDY()); 
    }
  fchi2 =fchi2x+fchi2y;



}
void TBTrack::AddHit(TBPlane* p,UShort_t i) 
      {fPlanes->AddLast(p);fHitidx[fPlanes->GetEntriesFast()-1]=i;}

void TBTrack::RemoveHit(UShort_t i)
{fPlanes->RemoveAt(i);fHitidx[i]=0;this->Fit();}




