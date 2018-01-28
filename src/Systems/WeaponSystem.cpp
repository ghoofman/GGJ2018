#include "WeaponSystem.h"
#include "../Globals.h"

#include "../Components/TransformComponent.h"
#include "../Components/WeaponComponent.h"
#include "../Components/DamageComponent.h"
#include "../Components/HealthComponent.h"

void WeaponSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {


	entityx::ComponentHandle<TransformComponent> transform, transform2;
	entityx::ComponentHandle<WeaponComponent> weapon;
	entityx::ComponentHandle<DamageComponent> damage;
	entityx::ComponentHandle<HealthComponent> health;
	for (entityx::Entity weaponEntity : es.entities_with_components(weapon, damage, transform)) {
        
        if(!weapon->hasTarget) {
            // Find a target
            for (entityx::Entity enemyEntity : es.entities_with_components(transform2, health)) {

                if(enemyEntity.has_component<InputComponent>()) {
                    // this is a player
                    continue;
                }

                OPfloat maxDist = 3.0;
                OPfloat dist = OPvec3Dist(transform->pos, transform2->pos);
                if (dist < maxDist) {
                    weapon->hasTarget = true;
                    weapon->targetId = enemyEntity.id();
                    if(weapon->projectile != NULL) {
                        weapon->projectile->alive = true;
                    }
                }
            }

        }


        if(weapon->hasTarget) {
            // Aim at target

            if(!EX.entities.valid(weapon->targetId)) {
                weapon->hasTarget = false;
                if(weapon->projectile != NULL) {
                    weapon->projectile->alive = false;
                }
                continue;
            }
            entityx::Entity e = EX.entities.get(weapon->targetId);

            if(!e.valid()) {
                weapon->hasTarget = false;
                if(weapon->projectile != NULL) {
                    weapon->projectile->alive = false;
                }
            } else {

                entityx::ComponentHandle<TransformComponent> enemyTransformComponent = e.component<TransformComponent>();
                entityx::ComponentHandle<HealthComponent> targetHealth = e.component<HealthComponent>();


                OPvec3 weaponPosition = transform->pos;
                OPvec3 enemyPosition = enemyTransformComponent->pos;

                OPvec3 diff = enemyPosition - weaponPosition;

                weapon->distanceToTarget = OPvec3Len(diff);

                if(OPvec3Len(diff) > 3.0) {
                    weapon->hasTarget = false;
                    if(weapon->projectile != NULL) {
                        weapon->projectile->alive = false;
                    }
                } else {

                    diff.z = 0;
                    OPvec3 diffNormal = OPvec3Norm(diff);

                    // Set rotation towards enemy


                    OPvec3 _rot = OPvec3(0, 0, 0);


                    OPfloat angleOnY = OPvec3AngleToTarget(weaponPosition, OPvec3(0,0,1), enemyPosition);
                    OPfloat angleOnX = OPvec3AngleToTarget(weaponPosition, OPvec3(1,0,0), enemyPosition);



                    _rot.y = -angleOnY;

                    if(weaponPosition.x > enemyPosition.x) {
                        if(_rot.y > 0.7) {
                            _rot.y = 0.7;
                        }
                        angleOnX = OPvec3Angle( OPvec3(-1,0,0), diffNormal);
                    } else {
                        if(_rot.y < -0.7) {
                            _rot.y = -0.7;
                        }
                        angleOnX = OPvec3Angle( OPvec3(1,0,0), diffNormal);
                    }


                    _rot.x = -angleOnX;
                    // if(weaponPosition.x > enemyPosition.x) {
                    //     if(_rot.y > 0.7) {
                    //         _rot.y = 0.7;
                    //     }
                    // } else {
                    //     if(_rot.y < -0.7) {
                    //         _rot.y = -0.7;
                    //     }
                    // }



                    // if(weaponPosition.y > enemyPosition.y) {
                    //     _rot.x = angleOnX;
                    //     // if(_rot.x < -0.7) {
                    //     //     _rot.x = -0.7;
                    //     // }
                    // } else {
                    //     _rot.x = angleOnX;
                    //     // if(_rot.x > 0.7) {
                    //     //     _rot.x = 0.7;
                    //     // }
                    // }


                    //transform->q = OPquat::From(OPmat4RotX(_rot.x) * OPmat4RotY(_rot.y));
                    transform->q = OPquat::From(OPmat4RotY(_rot.y) * OPmat4RotX(_rot.x));
                    // OPquat::CreateLookAt(weaponPosition, enemyPosition);
                        // OPquat::CreateRot(OPvec3(1, 0, 0), _rot.x) *
                        // OPquat::CreateRot(OPvec3(0, 1, 0), _rot.y);

                    // OPfloat angle = OPvec3Angle(OPVEC3_FORWARD, OPvec3Norm(position));
                    // Rotation.x = -angle;
                    // Rotation.y = OPvec3Angle(OPVEC3_LEFT, OPvec3Norm(position));
                    // Rotation.z = OPvec3Angle(OPVEC3_UP, OPvec3Norm(position));


                    damage->elapsedSinceLast += dt;
                    if(damage->elapsedSinceLast > damage->dps) {
                        damage->elapsedSinceLast -= damage->dps;

                        targetHealth->health -= damage->damage;
                        HitSound();

                        OPlogErr("Dealing Damage: %d | %d", damage->damage, targetHealth->health);
                    }


                }

            }
        }

	}

}