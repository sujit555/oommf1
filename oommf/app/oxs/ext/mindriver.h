/* FILE: mindriver.h            -*-Mode: c++-*-
 *
 * Example minimization Oxs_Driver class.
 *
 */

#ifndef _OXS_MINDRIVER
#define _OXS_MINDRIVER

#include "simstate.h"
#include "key.h"
#include "driver.h"
#include "minevolver.h"

/* End includes */

class Oxs_MinDriver:public Oxs_Driver {
private:
  Oxs_OwnedPointer<Oxs_MinEvolver> evolver_obj; // Evolver basket
  Oxs_Key<Oxs_MinEvolver> evolver_key;

  vector<OC_REAL8m> stopping_mxHxm;  // A/m

  Oxs_Output* max_mxHxm_output_obj_ptr; // Pointer to object providing
  /// max mxHxm data.  This is needed to determine StageDone events.

  Oxs_Output* total_energy_output_obj_ptr; // Pointer to output object
  /// providing "Total energy" data.  This is needed in some cases for
  /// initialization of new states.

  // Done checks, called by parent Oxs_Driver::IsStageDone and
  // Oxs_Driver::IsRunDone functions.
  virtual OC_BOOL ChildIsStageDone(const Oxs_SimState& state) const;
  virtual OC_BOOL ChildIsRunDone(const Oxs_SimState& state) const;

  // Disable copy constructor and assignment operator by declaring
  // them without defining them.
  Oxs_MinDriver(const Oxs_MinDriver&);
  Oxs_MinDriver& operator=(const Oxs_MinDriver&);

public:
  Oxs_MinDriver(const char* name,    // Child instance id
                 Oxs_Director* newdtr, // App director
                 const char* argstr);  // MIF input block parameters
  virtual const char* ClassName() const; // ClassName() is
  /// automatically generated by the OXS_EXT_REGISTER macro.
  virtual OC_BOOL Init();
  virtual ~Oxs_MinDriver();

  virtual void StageRequestCount(unsigned int& min,
				 unsigned int& max) const;
  // Number of stages wanted by driver

  // Generic interface
  virtual Oxs_ConstKey<Oxs_SimState> GetInitialState() const;

  OC_REAL8m GetTotalEnergy(const Oxs_SimState& tstate) const;
  /// Support function for Fill*StateDerivedData members

  virtual void FillStateMemberData(const Oxs_SimState& old_state,
                                   Oxs_SimState& new_state) const;
  virtual void FillStateDerivedData(const Oxs_SimState& old_state,
                                    const Oxs_SimState& new_state) const;

  // Use FillNewStageStateMemberData routine from parent

  virtual void FillNewStageStateDerivedData(const Oxs_SimState& old_state,
                                            int new_stage_number,
                                            const Oxs_SimState& new_state) const;

  virtual OC_BOOL InitNewStage(Oxs_ConstKey<Oxs_SimState> state,
                            Oxs_ConstKey<Oxs_SimState> prevstate);

  virtual  OC_BOOL
  Step(Oxs_ConstKey<Oxs_SimState> current_state,
       const Oxs_DriverStepInfo& step_info,
       Oxs_ConstKey<Oxs_SimState>& next_state);
  // Returns true if step was successful, false if
  // unable to step as requested.

};

#endif // _OXS_MINDRIVER
