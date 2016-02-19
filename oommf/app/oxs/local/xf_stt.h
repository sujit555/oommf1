/* FILE: xf_stt.h         -*-Mode: c++-*-
 *
 * Spin-transfer torque (STT), derived from Oxs_Energy class,
 * specified from a Tcl proc.
 *
 */

#ifndef _XF_STT
#define _XF_STT

#include <vector>

#include "director.h"
#include "energy.h"
#include "simstate.h"
#include "tclcommand.h"
#include "threevector.h"
#include "util.h"
#include "vectorfield.h"
#include "scalarfield.h"
#include "mesh.h"
#include "rectangularmesh.h"
#include "key.h"

OC_USE_STD_NAMESPACE;

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
typedef Oxs_TclCommandLineOption Nb_TclCommandLineOption;
typedef Oxs_TclCommand Nb_TclCommand;
typedef Oxs_SplitList Nb_SplitList;
#define Nb_ParseTclCommandLineRequest(a,b,c) Oxs_ParseTclCommandLineRequest((a),(b),(c))
#endif
//////////////////////////////////////////////////////////////////

class Xf_STT:public Oxs_Energy {
private:
  mutable OC_UINT4m mesh_id;
  OC_UINT4m number_of_stages;

  // Spin-transfer polarization field data structures
  Oxs_OwnedPointer<Oxs_ScalarField> P_fixed_init;
  Oxs_OwnedPointer<Oxs_ScalarField> P_free_init;
  Oxs_OwnedPointer<Oxs_ScalarField> Lambda_fixed_init;
  Oxs_OwnedPointer<Oxs_ScalarField> Lambda_free_init;
  Oxs_OwnedPointer<Oxs_ScalarField> eps_prime_init;
  Oxs_OwnedPointer<Oxs_ScalarField> J_init;
  Oxs_OwnedPointer<Oxs_VectorField> mp_init;

  mutable Oxs_MeshValue<OC_REAL8m> beta_q_plus;
  mutable Oxs_MeshValue<OC_REAL8m> beta_q_minus;
  mutable Oxs_MeshValue<OC_REAL8m> A;
  mutable Oxs_MeshValue<OC_REAL8m> B;
  mutable Oxs_MeshValue<OC_REAL8m> beta_eps_prime;
  mutable Oxs_MeshValue<ThreeVector> mp;
  OC_REAL8m ave_J; // Average value of J, excluding J_profile term.

  // Additional outputs
  void UpdateSpinTorqueOutputs(const Oxs_SimState&);
  Oxs_ScalarOutput<Xf_STT> ave_J_output;
  Oxs_VectorFieldOutput<Xf_STT> Jmp_output;

  // Scratch space
  Oxs_MeshValue<OC_REAL8m> stmpA;

  // Current flow direction.  Computed "free layer thickness" is
  // actually just cell size dimension parallel to current flow.
  // Current flow direction is also used to propagate mp, if enabled.
  enum JDirection { JD_INVALID,
                    JD_X_POS, JD_X_NEG,
                    JD_Y_POS, JD_Y_NEG,
                    JD_Z_POS, JD_Z_NEG };
  JDirection J_direction;

  // Note: If enabled, propagation direction is in the direction
  // of electron flow, i.e., directly opposite J.
  OC_BOOL mp_propagate;


  OC_BOOL fourpt_derivative; // If true, compute dm/dx using 4pt rule:
  /// [1,-8,0,8,-1]/12h.  Otherwise, use 2 pt rule: [-1,0,1]/2h.

  void UpdateMeshArrays(const Oxs_Mesh*) const;

  void Propagate_mp(const Oxs_Mesh* mesh,
                    const Oxs_MeshValue<ThreeVector>& spin) const;

  // Support for time varying current:
  OC_BOOL has_J_profile;
  vector<Nb_TclCommandLineOption> J_profile_opts;
  Nb_TclCommand J_profile_cmd;
  OC_REAL8m EvaluateJProfileScript(OC_UINT4m stage,OC_REAL8m stage_time,
                                OC_REAL8m total_time) const;

protected:
  virtual void GetEnergy(const Oxs_SimState& state,
			 Oxs_EnergyData& oed) const;

public:
  virtual const char* ClassName() const; // ClassName() is
  /// automatically generated by the OXS_EXT_REGISTER macro.
  Xf_STT(const char* name,     // Child instance id
		      Oxs_Director* newdtr, // App director
		      const char* argstr);  // MIF input block parameters
  virtual ~Xf_STT();
  virtual OC_BOOL Init();
  virtual void StageRequestCount(unsigned int& min,
				 unsigned int& max) const;
};


#endif // _XF_STT
