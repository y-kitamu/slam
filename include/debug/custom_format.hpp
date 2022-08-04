/**
 * @file custom_format.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-02 10:16:39
 */
#ifndef CUSTOM_FORMAT_HPP__
#define CUSTOM_FORMAT_HPP__

#include <fmt/format.h>

#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>

template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
struct fmt::formatter<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>> {
    // Presentation format: 'f' - full, 'p' - only prooperty.
    char presentation = 'f';

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'p')) presentation = *it++;
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& val,
                FormatContext& ctx) const -> decltype(ctx.out()) {
        if (presentation == 'p') {
            return fmt::format_to(ctx.out(), "rows = {}, cols = {}", val.rows(), val.cols());
        }
        std::string elems = "";
        for (int i = 0; i < val.rows(); i++) {
            for (int j = 0; j < val.cols(); j++) {
                elems += fmt::format("{} ", val(i, j));
            }
            elems += "\n";
        }
        return fmt::format_to(ctx.out(), "rows = {}, cols = {}, elems =\n{}", val.rows(), val.cols(),
                              elems);
    }
};

template <>
struct fmt::formatter<cv::Mat> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const cv::Mat& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "rows = {}, cols = {}, channels = {}, type = {}", mat.rows,
                              mat.cols, mat.channels(), getTypeName(mat.type()));
    }

    std::string getTypeName(int type) const {
        switch (type & CV_DEPTH_MAX) {
            case CV_8U:
                return "8U";
            case CV_8S:
                return "8S";
            case CV_16U:
                return "16U";
            case CV_16S:
                return "16S";
            case CV_32S:
                return "32S";
            case CV_32F:
                return "32F";
            case CV_64F:
                return "64F";
            default:
                return "unknown";
        }
    }
};


#endif  // CUSTOM_FORMAT_HPP__
