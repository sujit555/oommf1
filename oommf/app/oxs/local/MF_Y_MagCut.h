/* FILE: MF_Y_MagCut.h                 -*-Mode: c++-*-
 * version 1.1.1
 *
 * Class allows cutting magnetisation value in y direction of selected area
 *  
 * author: Marek Frankowski
 * contact: mfrankow[at]agh.edu.pl
 * website: layer.uci.agh.edu.pl/M.Frankowski
 *
 * This code is public domain work based on other public domains contributions
 */

#ifndef _MF_Y_MagCut
#define _MF_Y_MagCut

#include <string>
#include <vector>

#include "oc.h"
#include "director.h"
#include "energy.h"
#include "meshvalue.h"
#include "simstate.h"
#include "threevector.h"

OC_USE_STD_NAMESPACE;
OC_USE_STRING;

/* End includes */

//////////////////////////////////////////////////////////////////
// Old OOMMF version support
#ifndef OC_HAS_INT8
// Typedefs and defines for OOMMF prior to 16-Jul-2010
typedef REAL8m OC_REAL8m;
typedef UINT4m OC_UINT4m;
typedef BOOL OC_BOOL;
typedef INT4m OC_INT4m;
typedef INT4m OC_INT4m;
typedef UINT4m OC_INDEX;  // Supports pre-OC_INDEX API
#define OC_REAL8_EPSILON REAL8_EPSILON
#endif

#if !defined(OOMMF_API_INDEX) || OOMMF_API_INDEX<20110628
// Old API being used
#include "tclcommand.h"
typedef Oxs_TclCommandLineOption Nb_TclCommandLineOption;
typedef Oxs_TclCommand Nb_TclCommand;
typedef Oxs_SplitList Nb_SplitList;
#define Nb_ParseTclCommandLineRequest(a,b,c) Oxs_ParseTclCommandLineRequest((a),(b),(c))
#endif
//////////////////////////////////////////////////////////////////

// MSC++ 5.0 doesn't like embedded classes used for
// vector template types.  So, we just define this
// outside, with a really long name. <g>


class MF_Y_MagCut:public Oxs_Energy {
private:

  mutable OC_UINT4m mesh_id;

  // Supplied outputs, in addition to those provided by Oxs_Energy.

Oxs_ScalarOutput<MF_Y_MagCut> m_area_y_output;
OC_REAL8m area_x, area_y, area_z, delta_x, delta_y, delta_z;

protected:
  virtual void GetEnergy(const Oxs_SimState& state,
			 Oxs_EnergyData& oed) const;

public:
  virtual const char* ClassName() const; // ClassName() is
  /// automatically generated by the OXS_EXT_REGISTER macro.
  MF_Y_MagCut(const char* name,     // Child instance id
			 Oxs_Director* newdtr, // App director
			 const char* argstr);  // MIF input block parameters
  virtual ~MF_Y_MagCut();
  virtual OC_BOOL Init();
void UpdateDerivedOutputs(const Oxs_SimState& state);
};


#endif // _MF_mag_cut
