#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;

/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X + worldModel->getUNum();
    beamY = 0;
    beamAngle = 0;
}


SkillType NaoBehavior::selectSkill() {

    // My position and angle
    //cout << worldModel->getUNum() << ": " << worldModel->getMyPosition() << ",\t" << worldModel->getMyAngDeg() << "\n";

    // Position of the ball
    //cout << worldModel->getBall() << "\n";

    // Example usage of the roboviz drawing system and RVSender in rvdraw.cc.
    // Agents draw the position of where they think the ball is
    // Also see example in naobahevior.cc for drawing agent position and
    // orientation.
    /*
    worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    worldModel->getRVSender()->drawPoint("ball", ball.getX(), ball.getY(), 10.0f, RVSender::MAGENTA);
    */

    // ### Demo Behaviors ###

    // Walk in different directions
    //return goToTargetRelative(VecPosition(1,0,0), 0); // Forward
    //return goToTargetRelative(VecPosition(-1,0,0), 0); // Backward
    //return goToTargetRelative(VecPosition(0,1,0), 0); // Left
    //return goToTargetRelative(VecPosition(0,-1,0), 0); // Right
    //return goToTargetRelative(VecPosition(1,1,0), 0); // Diagonal
    //return goToTargetRelative(VecPosition(0,1,0), 90); // Turn counter-clockwise
    //return goToTargetRelative(VecPdosition(0,-1,0), -90); // Turn clockwise
    //return goToTargetRelative(VecPosition(1,0,0), 15); // Circle

    // Walk to the ball
    //return goToTarget(ball);

    // Turn in place to face ball
    /*double distance, angle;
    getTargetDistanceAndAngle(ball, distance, angle);
    if (abs(angle) > 10) {
      return goToTargetRelative(VecPosition(), angle);
    } else {
      return SKILL_STAND;
    }*/

    // Walk to ball while always facing forward
    //return goToTargetRelative(worldModel->g2l(ball), -worldModel->getMyAngDeg());

    // Dribble ball toward opponent's goal
    //return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, 0, 0));

    // Kick ball toward opponent's goal
    //return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X, 0, 0)); // Basic kick
    //return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Just stand in place
    //return SKILL_STAND;

    // Demo behavior where players form a rotating circle and kick the ball
    // back and forth
    //return demoKickingCircle();
    return mySkill();
}


SkillType NaoBehavior::mySkill() {

    VecPosition posball = worldModel->getBall();
    double ballkeeperX = -HALF_FIELD_X;
    double ballkeeperY = HALF_GOAL_Y / HALF_FIELD_Y * posball.getY();
    

    VecPosition Target[6] = {
        posball + VecPosition(-0.5,0),
        VecPosition (ballkeeperX , ballkeeperY , 0),
        posball + VecPosition(9,4,0),
        posball + VecPosition(9,-4,0),       
        VecPosition((posball.getX() - HALF_FIELD_X)/2,(posball.getY()/2) + 5 , 0),
        VecPosition((posball.getX() - HALF_FIELD_X)/2,(posball.getY()/2) - 5 , 0)
    };

    for(int i = 0 ; i < 6 ; i ++ ){

        if (Target[i].getX() > HALF_FIELD_X){
            Target[i].setX(HALF_FIELD_X);
        }
        if (Target[i].getX() < -HALF_FIELD_X){
            Target[i].setX(-HALF_FIELD_X);
        }
        if (Target[i].getY() < -HALF_FIELD_Y){
            Target[i].setY(-HALF_FIELD_Y);
        }
        if (Target[i].getY() > HALF_FIELD_Y) {
            Target[i].setY(HALF_FIELD_Y);
        }       
    }

    vector <vector<double>>  dis(6,vector<double>(6,0));
    int BotForTarget[6] = {0};
    VecPosition Pos;

    for ( int i = 0 ; i < 6 ; i ++){
        for ( int j = 0 ; j < 6 ; j ++){

            int playerNum = WO_TEAMMATE1 + j ;

            WorldObject *tem = worldModel->getWorldObject(playerNum);

            if (worldModel->getUNum() == playerNum){

                Pos = me ;
            }
            else {
                Pos = tem->pos;
            }

            dis[i][j] = Target[i].getDistanceTo(Pos);

            if(worldModel->getFallenTeammate(playerNum)){

                dis[i][j] ++ ;
            }
      
        }     
    }


    for (int i = 0 ; i < 6 ; i ++){

        int robot = min_element(dis[i].begin() , dis[i].begin() + 6) - dis[i].begin();

        BotForTarget[i] = robot ;

        for (int j = 0 ; j < 6 ; j ++){

            dis[j][robot] = 10000;
        }
    }
    for ( int i = 0 ; i < 6 ; i ++){
        worldModel->getRVSender()->clearStaticDrawings();
        worldModel->getRVSender()->drawPoint("ball",Target[i].getX(),Target[i].getY(),10.0f,RVSender::MAGENTA);   
    }
    

    for ( int i = 0 ; i < 6 ; i ++) {

        if (worldModel->getUNum() == WO_TEAMMATE1 + BotForTarget[i]){

            if(i == 0) {

                if ( me.getDistanceTo(Target[0]) > 1){

                    return goToTarget(collisionAvoidance(true,false,false,1,.5,Target[0],true));

                }

                else {

                    if (posball.getDistanceTo(Target[2]) < 5 || posball.getDistanceTo(Target[3]) < 5){

                        if(posball.getDistanceTo(VecPosition(HALF_FIELD_X,HALF_GOAL_Y)) > 
                           posball.getDistanceTo(VecPosition(HALF_FIELD_X,-HALF_GOAL_Y))){
                            
                            return kickBall(KICK_FORWARD,VecPosition(HALF_FIELD_X,HALF_GOAL_Y));
                        }

                        else {

                            return kickBall(KICK_FORWARD,VecPosition(HALF_FIELD_X,-HALF_GOAL_Y));
                        }

                    }

                    else{

                        if (me.getX() < -HALF_FIELD_X+PENALTY_X && 
                            me.getY()< PENALTY_Y/2.0 && 
                            me.getY()>-PENALTY_Y/2.0){

                            VecPosition closestobjpos;
                            double closestobjposdis = 10000;

                            for (int i = WO_TEAMMATE1 ; i < WO_TEAMMATE6 ; i ++){
                                if ( worldModel->getUNum() == i - WO_TEAMMATE1 + 1){
                                    continue ;
                                }
                                else {
                                    WorldObject *teamate = worldModel->getWorldObject(i);
                                    if(teamate->validPosition == true){
                                        VecPosition temp = teamate->pos;
                                        temp.setZ(0);
                                        double distance = me.getDistanceTo(temp);
                                        if(distance < closestobjposdis){
                                            closestobjposdis = distance;
                                            closestobjpos = temp ;
                                        }
                                    }
                                }
                            }

                            return kickBall(KICK_FORWARD,closestobjpos);
                        }

                        else {
                            
                            if ( me.getDistanceTo(Target[2]) < me.getDistanceTo(Target[3])){
                                return kickBall(KICK_FORWARD,Target[2]);
                            }
                            else {
                                return kickBall(KICK_FORWARD,Target[3]);
                            }
                        }
                    }
                }
            }
            else {
                return goToTarget(collisionAvoidance(true,false,false,-1,.5,Target[i],true));
            }
        }    
    }
}


/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType NaoBehavior::demoKickingCircle() {
    // Parameters for circle
    VecPosition center = VecPosition(-HALF_FIELD_X/2.0, 0, 0);
    double circleRadius = 5.0;
    double rotateRate = 2.5;

    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for(int i = WO_TEAMMATE1; i < WO_TEAMMATE1+NUM_AGENTS; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        // Have closest player kick the ball toward the center
        return kickBall(KICK_FORWARD, center);
    } else {
        // Move to circle position around center and face the center
        VecPosition localCenter = worldModel->g2l(center);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

        // Our desired target position on the circle
        // Compute target based on uniform number, rotate rate, and time
        VecPosition target = center + VecPosition(circleRadius,0,0).rotateAboutZ(360.0/(NUM_AGENTS-1)*(worldModel->getUNum()-(worldModel->getUNum() > playerClosestToBall ? 1 : 0)) + worldModel->getTime()*rotateRate);

        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);

        if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10) {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        } else if (me.getDistanceTo(target) < .5) {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
        } else {
            // Move toward target location
            return goToTarget(target);
        }
    }
}


