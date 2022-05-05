#pragma once

#include <Eigen/Core>


namespace simox::math
{

    /**
     * @brief A linear regression model of the form y = a + b * x,
     * or per dimension, y_i = a_i + b_i * x.
     *
     * - x is the scalar input variable (e.g. time)
     * - y is 3D vector output variable (e.g. position)
     * - a is a 3D bias vector
     * - b is a 3D slope vector
     *
     * In matrix notation, the equation system represented by the model is:
     *
     * [[ a_0  b_0 ]    [ 1 ]   [ y_0 ]
     *  [ a_1  b_1 ]  * [ x ] = [ y_1 ]
     *  [ a_2  b_2 ]]           [ y_2 ]
     *
     * Given data x[j] in R and y[j] \in R^3 (j = 0 .. n-1),
     * the regression solves the following equation system(s)
     * for [ a_i, b_i ] (i = 0 .. (3-1)):
     *
     * [[ 1  x[0]   ]             [  y[0]_i  ]
     *  [   ...     ]   [ a_i ]   [   ...    ]
     *  [ 1  x[j]   ] * [ b_i ] = [  y[j]_i  ]
     *  [   ...     ]             [   ...    ]
     *  [ 1  x[n-1] ]]            [ y[n-1]_i ]
     */
    class LinearRegression3D
    {
    public:

        using CoefficientsMatrix = Eigen::Matrix<double, 3, 2>;

        /**
         * The coefficients of the bias term and input variable x
         * [[ a_0  b_0 ]
         *  [ a_1  b_1 ]
         *  [ a_2  b_2 ]]
         */
        CoefficientsMatrix coefficients = CoefficientsMatrix::Zero();

        /// The input offset, so the virtual input x' = x + offset.
        double inputOffset = 0;


        /**
         * @brief Fit a linear regression model to the given data.
         * @param xs The input variables.
         * @param ys The output variables.
         * @param offsetInput If true, the inputs are offset to x' = x - x_0.
         * @return The regression model.
         */
        static LinearRegression3D
        Fit(const std::vector<double>& xs,
            const std::vector<Eigen::Vector3d>& ys,
            bool offsetInput = false);

        /**
         * @brief Predict the output variable of the given input variable.
         * @param x The input variable.
         * @return The predicted output variable.
         */
        Eigen::Vector3d predict(double x) const;

    };


    std::ostream& operator<<(std::ostream& os, const LinearRegression3D& r);

}
