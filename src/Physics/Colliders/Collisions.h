//
// Created by Mohamad on 28/07/2024.
//

#ifndef GLCPP_COLLISIONS_H
#define GLCPP_COLLISIONS_H
#include <glm/glm.hpp>
#include "SphereCollider.h"
#include "CubeCollider.h"
#include "PlaneCollider.h"
#include "CapsuleCollider.h"
namespace SGE {
    namespace PHYSICS {

        class Collisions {
        public:
            static bool intersectsSphereWithSphere(const SphereCollider &sphere1, const SphereCollider &sphere2);
            static bool intersectsSphereWithCube(const SphereCollider &sphere, const CubeCollider &cube);
            static bool intersectsSphereWithPlane(const SphereCollider &sphere, const PlaneCollider &plane);
            static bool intersectsSphereWithCapsule(const SphereCollider &sphere, const CapsuleCollider &capsule);

            static bool intersectsCubeWithCube(CubeCollider cube1,  CubeCollider &cube2);
            static bool intersectsCubeWithPlane(CubeCollider cube, const PlaneCollider &plane);
            static bool intersectsCubeWithCapsule(const CubeCollider &cube, const CapsuleCollider &capsule);
            static bool intersectsPlaneWithPlane(PlaneCollider &collider1, PlaneCollider collider2);


            static bool intersectsPlaneWithCapsule(const PlaneCollider &plane, const CapsuleCollider &capsule);

            static bool intersectsCapsuleWithCapsule(const CapsuleCollider &capsule1, const CapsuleCollider &capsule2);

            static float closestPointsBetweenSegments(const glm::vec3 &p1, const glm::vec3 &q1, const glm::vec3 &p2,
                                               const glm::vec3 &q2,
                                               glm::vec3 &c1, glm::vec3 &c2);

            static bool checkCapsuleCubeVertexCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd,
                                                 float capsuleRadius,
                                                 CubeCollider cube);



            bool checkCapsuleCubeEdgeCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd,
                                               float capsuleRadius,
                                               const CubeCollider &cube);

            static bool checkCapsuleCubeEdgeCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd,
                                               float capsuleRadius,
                                               CubeCollider cube, CapsuleCollider capsule);

            static bool checkCapsuleCubeFaceCollision(glm::vec3 capsuleStart, glm::vec3 capsuleEnd, float capsuleRadius,
                                                      CubeCollider cube);

            static bool overlapIntervals(float minA, float maxA, float minB, float maxB, float margin);
        };

    } // PHYSICS
} // SGE

#endif //GLCPP_COLLISIONS_H
