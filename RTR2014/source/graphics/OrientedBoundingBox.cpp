#include "graphics/OrientedBoundingBox.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
/**
 * \brief Eigenvalue decomposition of a symmetric matrix with the cyclic Jacobi method.
 *
 * The eigenvalues are returned in descending order, the eigenvectors are the columns of the
 * returned matrix.
 */
void eigenDecomposition(const glm::mat3& matrix, glm::vec3& eigenvalues, glm::mat3& eigenvectors)
{
    // Rows and columns are indexed by row first
    float a[3][3];
    for (int column = 0; column < 3; ++column)
    {
        for (int row = 0; row < 3; ++row)
        {
            a[row][column] = matrix[column][row];
        }
    }

    float v[3][3] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

    for (int sweep = 0; sweep < 32; ++sweep)
    {
        float offDiagonal = std::fabs(a[0][1]) + std::fabs(a[0][2]) + std::fabs(a[1][2]);
        if (offDiagonal < 1.0e-10f)
        {
            break;
        }

        for (int p = 0; p < 2; ++p)
        {
            for (int q = p + 1; q < 3; ++q)
            {
                if (std::fabs(a[p][q]) < 1.0e-12f)
                {
                    continue;
                }

                float theta = 0.5f * (a[q][q] - a[p][p]) / a[p][q];
                float t = 1.0f / (std::fabs(theta) + std::sqrt(theta * theta + 1.0f));
                if (theta < 0.0f)
                {
                    t = -t;
                }
                float c = 1.0f / std::sqrt(t * t + 1.0f);
                float s = t * c;
                float tau = s / (1.0f + c);
                float h = t * a[p][q];

                a[p][p] -= h;
                a[q][q] += h;
                a[p][q] = 0.0f;
                a[q][p] = 0.0f;

                for (int k = 0; k < p; ++k)
                {
                    float g = a[k][p];
                    float akq = a[k][q];
                    a[k][p] = g - s * (akq + g * tau);
                    a[k][q] = akq + s * (g - akq * tau);
                }
                for (int k = p + 1; k < q; ++k)
                {
                    float apk = a[p][k];
                    float hk = a[q][k];
                    a[p][k] = apk - s * (hk + apk * tau);
                    a[q][k] = hk + s * (apk - hk * tau);
                }
                for (int k = q + 1; k < 3; ++k)
                {
                    float apk = a[p][k];
                    float hk = a[q][k];
                    a[p][k] = apk - s * (hk + apk * tau);
                    a[q][k] = hk + s * (apk - hk * tau);
                }
                for (int k = 0; k < 3; ++k)
                {
                    float vkp = v[k][p];
                    float vkq = v[k][q];
                    v[k][p] = vkp - s * (vkq + vkp * tau);
                    v[k][q] = vkq + s * (vkp - vkq * tau);
                }
            }
        }
    }

    float values[3] = {a[0][0], a[1][1], a[2][2]};
    int order[3] = {0, 1, 2};
    // Sort by descending eigenvalue
    for (int i = 0; i < 2; ++i)
    {
        for (int j = i + 1; j < 3; ++j)
        {
            if (values[order[j]] > values[order[i]])
            {
                int swap = order[i];
                order[i] = order[j];
                order[j] = swap;
            }
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        eigenvalues[i] = values[order[i]];
        for (int row = 0; row < 3; ++row)
        {
            eigenvectors[i][row] = v[row][order[i]];
        }
    }
}
} // namespace

OrientedBoundingBox::OrientedBoundingBox()
    : m_center(0.0f), m_axes(1.0f), m_halfExtents(0.0f)
{
    return;
}

OrientedBoundingBox::OrientedBoundingBox(const glm::vec3& center, const glm::mat3& axes,
                                         const glm::vec3& halfExtents)
    : m_center(center), m_axes(axes), m_halfExtents(halfExtents)
{
    return;
}

OrientedBoundingBox OrientedBoundingBox::fromPoints(const std::vector<glm::vec3>& points)
{
    OrientedBoundingBox box;
    if (points.empty())
    {
        return box;
    }

    glm::vec3 centroid(0.0f);
    for (const glm::vec3& point : points)
    {
        centroid += point;
    }
    centroid /= static_cast<float>(points.size());

    glm::mat3 covariance(0.0f);
    for (const glm::vec3& point : points)
    {
        glm::vec3 delta = point - centroid;
        covariance[0][0] += delta.x * delta.x;
        covariance[0][1] += delta.x * delta.y;
        covariance[0][2] += delta.x * delta.z;
        covariance[1][0] += delta.y * delta.x;
        covariance[1][1] += delta.y * delta.y;
        covariance[1][2] += delta.y * delta.z;
        covariance[2][0] += delta.z * delta.x;
        covariance[2][1] += delta.z * delta.y;
        covariance[2][2] += delta.z * delta.z;
    }

    glm::vec3 eigenvalues(0.0f);
    glm::mat3 axes(1.0f);
    eigenDecomposition(covariance, eigenvalues, axes);

    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (const glm::vec3& point : points)
    {
        glm::vec3 delta = point - centroid;
        for (int i = 0; i < 3; ++i)
        {
            float projection = glm::dot(delta, glm::vec3(axes[i]));
            minimum[i] = std::min(minimum[i], projection);
            maximum[i] = std::max(maximum[i], projection);
        }
    }

    glm::vec3 center = centroid;
    glm::vec3 halfExtents(0.0f);
    for (int i = 0; i < 3; ++i)
    {
        center += glm::vec3(axes[i]) * ((minimum[i] + maximum[i]) * 0.5f);
        halfExtents[i] = (maximum[i] - minimum[i]) * 0.5f;
    }

    box.m_center = center;
    box.m_axes = axes;
    box.m_halfExtents = halfExtents;
    return box;
}

OrientedBoundingBox OrientedBoundingBox::transformed(const glm::mat4& matrix) const
{
    OrientedBoundingBox box;
    box.m_center = glm::vec3(matrix * glm::vec4(m_center, 1.0f));

    // The axes of a box with a non uniform scale are no longer orthogonal, they are kept as the
    // directions of the transformed edges and the scale is folded into the extents.
    glm::mat3 linear = glm::mat3(matrix);
    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = linear * glm::vec3(m_axes[i]);
        float length = glm::length(axis);
        if (length > 0.0f)
        {
            box.m_axes[i] = axis / length;
            box.m_halfExtents[i] = m_halfExtents[i] * length;
        }
        else
        {
            box.m_axes[i] = glm::vec3(m_axes[i]);
            box.m_halfExtents[i] = 0.0f;
        }
    }
    return box;
}

bool OrientedBoundingBox::intersectsPlane(const glm::vec4& plane) const
{
    glm::vec3 normal(plane);
    glm::vec3 support = m_center;
    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::vec3(m_axes[i]);
        float projection = glm::dot(axis, normal);
        support += (projection >= 0.0f ? m_halfExtents[i] : -m_halfExtents[i]) * axis;
    }
    return glm::dot(normal, support) + plane.w >= 0.0f;
}
