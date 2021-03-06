#ifndef DSimPrimaryGenerator_hh_Seen
#define DSimPrimaryGenerator_hh_Seen

class G4Event;
#include <G4VPrimaryGenerator.hh>

class DSimVKinematicsGenerator;
class DSimVCountGenerator;
class DSimVPositionGenerator;
class DSimVTimeGenerator;

/// Generate one (or more) G4PrimaryVertex objects and add them to the
/// G4Event.  This is used by the DSimUserPrimaryGeneratorAction to generate
/// particles which will be tracked by the G4 simulation.  This uses the
/// DSimVKinematicsGenerator, ND20VCountGenerator, DSimVPositionGenerator,
/// and DSimVTimeGenerator classes to create the G4PrimaryVertexObjects.  The
/// control of the DSimPrimaryGenerator object is through the
/// DSimUserPrimaryGeneratorMessenger class.
class DSimPrimaryGenerator: public G4VPrimaryGenerator {
public:
    DSimPrimaryGenerator(DSimVKinematicsGenerator* kine,
                          DSimVCountGenerator* count,
                          DSimVPositionGenerator* position,
                          DSimVTimeGenerator* time);
    virtual ~DSimPrimaryGenerator();

    /// A pure virtual method to generate the actual primary particles which
    /// must be implemented in each derived class.
    virtual void GeneratePrimaryVertex(G4Event* evt);
    
    /// Return the name of this generator.
    G4String GetName();
    
    /// Return the kinematics generator;
    const DSimVKinematicsGenerator* GetKinematicsGenerator() const {
        return fKinematics;
    }

    /// Return the count generator.
    const DSimVCountGenerator* GetCountGenerator() const {
        return fCount;
    }

    /// Return the position generator.
    const DSimVPositionGenerator* GetPositionGenerator() const {
        return fPosition;
    }

    /// Return the time generator.
    const DSimVTimeGenerator* GetTimeGenerator() const {
        return fTime;
    }

private:
    /// The kinematics generator that will return the primary particles being
    /// simulated by this generator.  The kinematics generator adds the
    /// primary vertex to the G4Event along with the primary particles that
    /// eminate from the vertex.  The generator is passed a 4-vector position
    /// which gives the position that DSimVPrimaryGenerator would like an
    /// interaction generated for.  The kinematics generator can choose to
    /// ignore this information and create the vertex at any location, but it
    /// might be overriden by DSimVPrimaryGenerator (depending on which
    /// DSimVVertexGenerator is used).
    DSimVKinematicsGenerator* fKinematics;

    /// The count generator determines the number of primary vertices that
    /// will be added to a G4Event.
    DSimVCountGenerator* fCount;

    /// The position generator picks candidate positions for the primary
    /// vertices. The candidate vertices generated by the position generator
    /// are passed to the kinematics generator as an advisory position.  If
    /// the position generator returns true from
    /// DSimVPositionGenerator::ForcePosition(), then the vertex from the
    /// kinematics generator will be overridden by the DSimVPrimaryGenerator.
    DSimVPositionGenerator* fPosition;

    /// The time generator picks candidate times for the primary vertices.
    /// The candidate times are passed to the kinematics generator as advisory
    /// times.  If the time generator returns true from
    /// DSimVTimeGenerator::ForceTime(), then the vertex from the kinematics
    /// generator will be overridden by the DSimVPrimaryGenerator.
    DSimVTimeGenerator* fTime;
};
#endif
