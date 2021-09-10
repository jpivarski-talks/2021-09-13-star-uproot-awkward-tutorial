/**
 * \class StPicoMtdPidTraits
 * \brief Holds information about MTD-matched track
 *
 * The class stores information about related to the track
 * that matched the Muon Telescope Detector (MTD)
 */

#ifndef StPicoMtdPidTraits_h
#define StPicoMtdPidTraits_h

// ROOT headers
#include "TObject.h"

//_________________
class StPicoMtdPidTraits : public TObject {

 public:
  
  /// Default constructor
  StPicoMtdPidTraits();
  /// Copy constructor
  StPicoMtdPidTraits(const StPicoMtdPidTraits &traits);
  /// Destructor
  virtual ~StPicoMtdPidTraits();
  /// Print MTD PID traits information
  virtual void Print(const Char_t* option = "") const;

  //
  // Getters
  //

  /// Return assiciated track index
  Int_t    trackIndex()        const { return mTrackIndex; }
  /// Return hit index
  Int_t    mtdHitIndex()       const { return mMtdHitIndex; }
  /// Return hit channel
  Int_t    gChannel()          const { return mMtdHitChan; }
  /// Return backleg number
  Int_t    backleg()           const { return mMtdHitChan / 60 + 1; }
  /// Return module number
  Int_t    module()            const { return (mMtdHitChan % 60) / 12 + 1; }
  /// Return cell number
  Int_t    cell()              const { return mMtdHitChan % 12; }
  /// Return match flag
  Int_t    matchFlag()         const { return mMatchFlag; }
  /// Return delta Y
  Float_t  deltaY()            const { return (Float_t)mDeltaY / 200.; }
  /// Return delta Z
  Float_t  deltaZ()            const { return (Float_t)mDeltaZ / 200.; }
  /// Return difference between measured and expected time of flight
  Float_t  deltaTimeOfFlight() const { return mDeltaTimeOfFlight; }
  /// Return beta
  Float_t  beta()              const { return (Float_t)mBeta / 20000.; }

  //
  // Setters
  //

  /// Set track index of the associated track
  void setTrackIndex(Int_t index)   { mTrackIndex = (Short_t) index; }
  /// Set MTD hit index
  void setMtdHitIndex(Int_t index)  { mMtdHitIndex = (Short_t) index; }
  /// Set match flag
  void setMatchFlag(Char_t flag)    { mMatchFlag = (Char_t)flag; }
  /// Set delta Y
  void setDeltaY(Float_t dy);
  /// Set delta Z
  void setDeltaZ(Float_t dz);
  /// Set difference between measured and expected time of flight
  void setDeltaTimeOfFlight(Float_t t) { mDeltaTimeOfFlight = t; }
  /// Set beta
  void setBeta(Float_t beta);
  /// Set hit channel: (backleg-1) * 60 + (module-1) * 12 + cell
  void setHitChannel(Int_t backleg, Int_t module, Int_t cell);

 private:

  /// Index to the associated track in the event
  Short_t   mTrackIndex;
  /// Index to the associated MTD hit in the event
  Short_t   mMtdHitIndex;
  /// Matching flag indicating multiple matches
  Char_t    mMatchFlag;
  /// DeltaY between matched track-hit pair * 200 = provides 0.005 cm precision
  Short_t   mDeltaY;
  /// DeltaZ between matched track-hit pair * 200 = provides 0.005 cm precision
  Short_t   mDeltaZ;
  /// Difference between measured and expected time-of-flight
  Float_t   mDeltaTimeOfFlight;
  /// Beta of matched tracks * 20000
  UShort_t  mBeta;
  /// HitChan encoding: (backleg-1) * 60 + (module-1) * 12 + cell
  Short_t   mMtdHitChan;

  ClassDef(StPicoMtdPidTraits, 3)
};

#endif
