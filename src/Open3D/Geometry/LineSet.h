// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <Eigen/Core>
#include <memory>
#include <vector>

#include "Open3D/Geometry/Geometry3D.h"

namespace open3d {
namespace geometry {

class PointCloud;
class TriangleMesh;

class LineSet : public Geometry3D {
public:
    LineSet() : Geometry3D(Geometry::GeometryType::LineSet) {}
    ~LineSet() override {}

public:
    void Clear() override;
    bool IsEmpty() const override;
    Eigen::Vector3d GetMinBound() const override;
    Eigen::Vector3d GetMaxBound() const override;
    LineSet &Transform(const Eigen::Matrix4d &transformation) override;
    LineSet &Translate(const Eigen::Vector3d &translation) override;
    LineSet &Scale(const double scale, bool center = true) override;
    LineSet &Rotate(const Eigen::Vector3d &rotation,
                    bool center = true,
                    RotationType type = RotationType::XYZ) override;

public:
    LineSet &operator+=(const LineSet &lineset);
    LineSet operator+(const LineSet &lineset) const;

public:
    bool HasPoints() const { return points_.size() > 0; }

    bool HasLines() const { return HasPoints() && lines_.size() > 0; }

    bool HasColors() const {
        return HasLines() && colors_.size() == lines_.size();
    }

    std::pair<Eigen::Vector3d, Eigen::Vector3d> GetLineCoordinate(
            size_t line_index) const {
        return std::make_pair(points_[lines_[line_index][0]],
                              points_[lines_[line_index][1]]);
    }

    /// Assigns each line in the LineSet the same color \param color.
    void PaintUniformColor(const Eigen::Vector3d &color) {
        colors_.resize(lines_.size());
        for (size_t i = 0; i < lines_.size(); i++) {
            colors_[i] = color;
        }
    }

public:
    std::vector<Eigen::Vector3d> points_;
    std::vector<Eigen::Vector2i> lines_;
    std::vector<Eigen::Vector3d> colors_;
};

/// Factory function to create a LineSet from two PointClouds
/// (\param cloud0, \param cloud1) and a correspondence set
/// \param correspondences.
std::shared_ptr<LineSet> CreateLineSetFromPointCloudCorrespondences(
        const PointCloud &cloud0,
        const PointCloud &cloud1,
        const std::vector<std::pair<int, int>> &correspondences);

/// Factory function to create a LineSet from edges of a triangle mesh
/// \param mesh.
std::shared_ptr<LineSet> CreateLineSetFromTriangleMesh(
        const TriangleMesh &mesh);

}  // namespace geometry
}  // namespace open3d
