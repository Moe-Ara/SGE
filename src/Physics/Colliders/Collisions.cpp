//
// Created by Mohamad on 28/07/2024.
//

#include <glm/gtx/string_cast.hpp>
#include "Collisions.h"

namespace SGE {
    namespace PHYSICS {
        bool Collisions::intersectsSphereWithSphere(const SphereCollider &sphere1, const SphereCollider &sphere2) {
            glm::vec3 centerA = sphere1.getTransform().getMTranslation();
            glm::vec3 centerB = sphere2.getTransform().getMTranslation();
            float radiusA = sphere1.getMRadius();
            float radiusB = sphere2.getMRadius();
            float distance = glm::length(centerA - centerB);
            return distance < (radiusA + radiusB);
        }

        bool Collisions::intersectsSphereWithCube(const SphereCollider &sphere, const CubeCollider &cube) {
            auto cubeTransformMatrix = cube.getTransform().mat4();
            glm::vec3 sphereCenter = sphere.getTransform().getMTranslation();
            float sphereRadius = sphere.getMRadius();
            glm::mat4 invTransform = glm::inverse(cubeTransformMatrix);
            glm::vec3 localSphereCenter = glm::vec3(invTransform * glm::vec4(sphereCenter, 1.0f));
            glm::vec3 localMin = cube.calculateLocalMin();
            glm::vec3 localMax = cube.calculateLocalMax();
            glm::vec3 closestPointLocal = glm::clamp(localSphereCenter, localMin, localMax);
            glm::vec3 closestPointWorld = glm::vec3(cubeTransformMatrix * glm::vec4(closestPointLocal, 1.0f));
            float distance = glm::length(closestPointWorld - sphereCenter);
            return distance < sphereRadius;
        }

        bool Collisions::intersectsSphereWithPlane(const SphereCollider &sphere, const PlaneCollider &plane) {
            glm::vec3 sphereCenter = sphere.getTransform().getMTranslation();
            float sphereRadius = sphere.getMRadius();
            glm::vec3 planeNormal = plane.getTransform().normalMatrix()[1];
            glm::vec3 planePoint = plane.getTransform().getMTranslation();
            float distance = glm::dot(planeNormal, sphereCenter - planePoint);
            return std::abs(distance) <= sphereRadius;
        }

        bool Collisions::intersectsSphereWithCapsule(const SphereCollider &sphere, const CapsuleCollider &capsule) {
            glm::vec3 capsuleStart = capsule.getTransform().getMTranslation() + glm::vec3(0, capsule.getMHeight(), 0);
            glm::vec3 capsuleEnd = capsule.getTransform().getMTranslation() + capsule.getTransform().getMRotation() * glm::vec3(0, -capsule.getMHeight(), 0);
            glm::vec3 sphereCenter = sphere.getTransform().getMTranslation();
            float sphereRadius = sphere.getMRadius();
            float capsuleRadius = capsule.getMRadius();

            glm::vec3 capsuleDir = capsuleEnd - capsuleStart;
            float projectionFactor = glm::dot(sphereCenter - capsuleStart, capsuleDir) / glm::dot(capsuleDir, capsuleDir);
            projectionFactor = glm::clamp(projectionFactor, 0.0f, 1.0f);
            glm::vec3 closestPoint = capsuleStart + projectionFactor * capsuleDir;

            float distance = glm::length(closestPoint - sphereCenter);
            return distance < (sphereRadius + capsuleRadius);
        }

        bool Collisions::intersectsCubeWithCube(CubeCollider cube1, CubeCollider &cube2) {
            std::vector<glm::vec3> axes;
            axes.push_back(cube1.getTransform().getMRotation() * glm::vec3(1, 0, 0));
            axes.push_back(cube1.getTransform().getMRotation() * glm::vec3(0, 1, 0));
            axes.push_back(cube1.getTransform().getMRotation() * glm::vec3(0, 0, 1));
            axes.push_back(cube2.getTransform().getMRotation() * glm::vec3(1, 0, 0));
            axes.push_back(cube2.getTransform().getMRotation() * glm::vec3(0, 1, 0));
            axes.push_back(cube2.getTransform().getMRotation() * glm::vec3(0, 0, 1));


            axes.push_back(glm::cross(cube1.getTransform().getMRotation() * glm::vec3(1, 0, 0),
                                      cube2.getTransform().getMRotation() * glm::vec3(1, 0, 0)));
            axes.push_back(glm::cross(cube1.getTransform().getMRotation() * glm::vec3(1, 0, 0),
                                      cube2.getTransform().getMRotation() * glm::vec3(0, 0, 1)));
            axes.push_back(glm::cross(cube1.getTransform().getMRotation() * glm::vec3(0, 0, 1),
                                      cube2.getTransform().getMRotation() * glm::vec3(1, 0, 0)));
            axes.push_back(glm::cross(cube1.getTransform().getMRotation() * glm::vec3(0, 0, 1),
                                      cube2.getTransform().getMRotation() * glm::vec3(0, 0, 1)));

            for (const auto &axis: axes) {
                float minA, maxA;
                cube1.projectCube(axis, minA, maxA);

                float minB, maxB;
                cube2.projectCube(axis, minB, maxB);


                const float threshold = 0.05f;
                if (!(maxA + threshold >= minB && maxB + threshold >= minA)) {
                    return false;
                }
            }

            return true;
        }

        bool Collisions::intersectsCubeWithPlane(CubeCollider cube, const PlaneCollider &plane) {
            glm::vec3 planeNormal = plane.getMNormal();
            float planeDistance = plane.getMDistance();
            const auto &vertices = cube.getTransformedVertices();

            for (const auto &vertex: vertices) {
                float distance = glm::dot(vertex, planeNormal) - planeDistance;

                if (distance <= 0.0f) {
                    return true;
                }
            }

            return false;
        }
        // Function to find the closest point on a line segment to a point
        glm::vec3 closestPointOnLineSegment(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &point) {
            glm::vec3 ab = b - a;
            float t = glm::dot(point - a, ab) / glm::dot(ab, ab);
            t = glm::clamp(t, 0.0f, 1.0f);
            return a + t * ab;
        }
        bool Collisions::intersectsCubeWithCapsule(const CubeCollider &cube, const CapsuleCollider &capsule) {
            return false;
        }
        bool Collisions::checkCapsuleCubeEdgeCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd, float capsuleRadius, CubeCollider cube, CapsuleCollider capsule) {
            // Define cube face normals in local space
            std::vector<glm::vec3> faceNormals = {
                    glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
                    glm::vec3(0, 1, 0), glm::vec3(0, -1, 0),
                    glm::vec3(0, 0, 1), glm::vec3(0, 0, -1)
            };

            // Transform face normals to world space
            glm::mat3 rotation = glm::mat3(cube.getTransform().getMRotation());
            for (auto &normal : faceNormals) {
                normal = glm::normalize(rotation * normal);
            }

            for (const auto &normal : faceNormals) {
                float minA, maxA;
                SGE::PHYSICS::CapsuleCollider::projectCapsuleOnAxis(capsuleStart, capsuleEnd, capsuleRadius, normal, minA, maxA);

                float minB, maxB;
                cube.projectCube( normal, minB, maxB);
                auto threshold=0.05f;
                if (!overlapIntervals(minA, maxA, minB, maxB, threshold)) {
                    return false;
                }
            }

            return true;
        }

//        bool Collisions::checkCapsuleCubeEdgeCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd, float capsuleRadius, const CubeCollider &cube) {
//            // Get cube edges in world space
//            std::vector<std::pair<glm::vec3, glm::vec3>> edges = cube.getEdges();
//
//            for (const auto &edge : edges) {
//                glm::vec3 closestPointCapsule, closestPointEdge;
//                float distance = closestPointsBetweenSegments(capsuleStart, capsuleEnd, edge.first, edge.second, closestPointCapsule, closestPointEdge);
//
//                if (distance <= capsuleRadius) {
//                    return true;
//                }
//            }
//
//            return false;
//        }
//
//        bool Collisions::checkCapsuleCubeVertexCollision(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd, float capsuleRadius, CubeCollider cube) {
//            // Get cube vertices in world space
//            std::vector<glm::vec3> vertices = cube.getTransformedVertices();
//
//            for (const auto &vertex : vertices) {
//                float distance = distancePointSegment(vertex, capsuleStart, capsuleEnd);
//
//                if (distance <= capsuleRadius) {
//                    return true;
//                }
//            }
//
//            return false;
//        }

        bool Collisions::intersectsPlaneWithPlane(PlaneCollider &collider1, PlaneCollider collider2) {
//            glm::vec3 normal1 = collider1.getMNormal();
//            glm::vec3 point1 = collider1.getTransform().getMTranslation();
//            glm::vec3 normal2 = collider2.getMNormal();
//            glm::vec3 point2 = collider2.getTransform().getMTranslation();
//
//            if (glm::length(glm::cross(normal1, normal2)) < 1e-6f) {
//                // Planes are parallel
//                return std::abs(glm::dot(normal1, point2 - point1)) < 1e-6f;
//            } else {
//                // Check if planes intersect within their extents
//                glm::vec3 extentMin1 = collider1.getExtentMin();
//                glm::vec3 extentMax1 = collider1.getExtentMax();
//                glm::vec3 extentMin2 = collider2.getExtentMin();
//                glm::vec3 extentMax2 = collider2.getExtentMax();
//
//                return (extentMin1.x <= extentMax2.x && extentMax1.x >= extentMin2.x) &&
//                       (extentMin1.y <= extentMax2.y && extentMax1.y >= extentMin2.y) &&
//                       (extentMin1.z <= extentMax2.z && extentMax1.z >= extentMin2.z);
//            }
            return false;
        }

        bool Collisions::intersectsPlaneWithCapsule(const PlaneCollider &plane, const CapsuleCollider &capsule) {
            glm::vec3 capsuleStart = capsule.getTransform().getMTranslation() + glm::vec3(0, capsule.getMHeight(), 0);
            glm::vec3 capsuleEnd = capsule.getTransform().getMTranslation() + capsule.getTransform().getMRotation() * glm::vec3(0, -capsule.getMHeight(), 0);
            auto capsuleRadius = capsule.getMRadius();
            auto planeNormal=plane.getMNormal();
            glm::vec3 planePoint = plane.getTransform().getMTranslation();

            float distanceStart = glm::dot(planeNormal, capsuleStart - planePoint);
            float distanceEnd = glm::dot(planeNormal, capsuleEnd - planePoint);

            if(std::abs(distanceStart )<= capsuleRadius || std::abs(distanceEnd) <=capsuleRadius){
                return true;
            }
            if((distanceStart> 0 && distanceEnd<0) || (distanceStart< 0 && distanceEnd>0)){
                return true;
            }
            return false;
        }

        bool
        Collisions::intersectsCapsuleWithCapsule(const CapsuleCollider &capsule1, const CapsuleCollider &capsule2) {
            glm::vec3 start1 = capsule1.getTransform().getMTranslation() + glm::vec3(0, capsule1.getMHeight(), 0);
            glm::vec3 end1 = capsule1.getTransform().getMTranslation() + capsule1.getTransform().getMRotation() * glm::vec3(0, -capsule1.getMHeight(), 0);
            glm::vec3 start2 = capsule2.getTransform().getMTranslation() + glm::vec3(0, capsule2.getMHeight(), 0);
            glm::vec3 end2 = capsule2.getTransform().getMTranslation() + capsule2.getTransform().getMRotation() * glm::vec3(0, -capsule2.getMHeight(), 0);

            float radius1 = capsule1.getMRadius();
            float radius2 = capsule2.getMRadius();

            glm::vec3 closestPoint1, closestPoint2;
            float distance = closestPointsBetweenSegments(start1, end1, start2, end2, closestPoint1, closestPoint2);

            float bodySensitivityMargin = (radius1 + radius2) * 0.1f;
            float capSensitivityMargin = (radius1 + radius2) * 0.05f;

            bool onCaps1 = closestPoint1 == start1 || closestPoint1 == end1;
            bool onCaps2 = closestPoint2 == start2 || closestPoint2 == end2;

            float sensitivityMargin = (onCaps1 || onCaps2) ? capSensitivityMargin : bodySensitivityMargin;

            return distance <= (radius1 + radius2 )-sensitivityMargin;
        }
        float Collisions::closestPointsBetweenSegments(const glm::vec3 &p1, const glm::vec3 &q1, const glm::vec3 &p2, const glm::vec3 &q2, glm::vec3 &c1, glm::vec3 &c2) {
            glm::vec3 d1 = q1 - p1;
            glm::vec3 d2 = q2 - p2;
            glm::vec3 r = p1 - p2;
            float a = glm::dot(d1, d1);
            float e = glm::dot(d2, d2);
            float f = glm::dot(d2, r);

            float s, t;
            const float EPSILON = 0.0001f;

            if (a <= EPSILON && e <= EPSILON) {
                c1 = p1;
                c2 = p2;
                return glm::length(c1 - c2);
            }
            if (a <= EPSILON) {
                s = 0.0f;
                t = glm::clamp(f / e, 0.0f, 1.0f);
            } else {
                float c = glm::dot(d1, r);
                if (e <= EPSILON) {
                    t = 0.0f;
                    s = glm::clamp(-c / a, 0.0f, 1.0f);
                } else {
                    float b = glm::dot(d1, d2);
                    float denom = a * e - b * b;

                    if (denom != 0.0f) {
                        s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                    } else {
                        s = 0.0f;
                    }

                    t = (b * s + f) / e;

                    if (t < 0.0f) {
                        t = 0.0f;
                        s = glm::clamp(-c / a, 0.0f, 1.0f);
                    } else if (t > 1.0f) {
                        t = 1.0f;
                        s = glm::clamp((b - c) / a, 0.0f, 1.0f);
                    }
                }
            }

            c1 = p1 + d1 * s;
            c2 = p2 + d2 * t;
            return glm::length(c1 - c2);
        }

        bool
        Collisions::checkCapsuleCubeFaceCollision(glm::vec3 capsuleStart, glm::vec3 capsuleEnd, float capsuleRadius,
                                                  CubeCollider cube) {
            // Define the local face normals
            std::vector<glm::vec3> localFaceNormals = {
                    glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
                    glm::vec3(0, 1, 0), glm::vec3(0, -1, 0),
                    glm::vec3(0, 0, 1), glm::vec3(0, 0, -1)
            };

            // Get the normal matrix of the cube to transform the face normals to world space
            glm::mat3 normalMatrix = cube.getTransform().normalMatrix();
            std::vector<glm::vec3> faceNormals;
            for (const auto& localNormal : localFaceNormals) {
                faceNormals.push_back(glm::normalize(normalMatrix * localNormal));
            }

            for (const auto &axis : faceNormals) {
                float minA, maxA;
                SGE::PHYSICS::CapsuleCollider::projectCapsuleOnAxis(capsuleStart, capsuleEnd, capsuleRadius, axis, minA, maxA);

                float minB, maxB;
                cube.projectCube(axis, minB, maxB);

                if (!Collisions::overlapIntervals(minA, maxA, minB, maxB, 0.05f)) {
                    return false;
                }
            }

            return true;
        }
        bool Collisions::overlapIntervals(float minA, float maxA, float minB, float maxB, float margin) {
            return (maxA + margin >= minB) && (maxB + margin >= minA);
        }
    } // PHYSICS
} // SGE