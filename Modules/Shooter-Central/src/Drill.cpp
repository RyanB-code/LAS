#include "Drill.h"

using namespace ShooterCentral;

// MARK: DRILL INFO
Drill::Drill(std::string setName, CourseOfFire setCOF, bool setDryFire, bool setLiveFire)
            :   name            {setName},
                courseOfFire    { setCOF },
                canBeDryFire    { setDryFire },
                canBeLiveFire   { setLiveFire }            
{

}
Drill::~Drill(){

}