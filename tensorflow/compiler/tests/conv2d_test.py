# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Tests for Conv2D via the XLA JIT.

The canned results in these tests are created by running each test using the
Tensorflow CPU device and saving the output.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

# from tensorflow.compiler.tests.xla_test import XLATestCase
from tensorflow.python.client import session as session_lib
from tensorflow.python.platform import test
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import gen_nn_ops
from tensorflow.python.ops import nn_impl
from tensorflow.python.ops import nn_ops
from tensorflow.python.platform import googletest


class Conv2DTest(test.TestCase):

  def _VerifyValues(self, input_sizes, filter_sizes, stride, padding, expected):
    """Tests that tf.nn.conv2d produces the expected value.

    Args:
      input_sizes: Input tensor dimensions in
        [batch, input_rows, input_cols, input_depth].
      filter_sizes: Filter tensor dimensions in
        [kernel_rows, kernel_cols, input_depth, output_depth].
      stride: Stride.
      padding: Padding type.
      expected: Expected output.
    """
    total_size_1 = np.prod(input_sizes)
    total_size_2 = np.prod(filter_sizes)
    x1 = np.arange(1, total_size_1 + 1, dtype=np.float32).reshape(input_sizes)
    x2 = np.arange(1, total_size_2 + 1, dtype=np.float32).reshape(filter_sizes)
    strides = [1, stride, stride, 1]

    with session_lib.Session() as sess:
      with self.test_session() as sess:
      # with self.test_scope():
        t1 = array_ops.placeholder(dtypes.float32, shape=input_sizes)
        t2 = array_ops.placeholder(dtypes.float32, shape=filter_sizes)
        with ops.device("device:XLA_GPU:0"):
          out = nn_ops.conv2d(
              t1, t2, strides=strides, padding=padding, data_format="NHWC")
    
          value = sess.run(out, {t1: x1, t2: x2})
      print (value)
      self.assertArrayNear(expected, np.ravel(value), 1e-3)

  def testConv2D1x1Filter(self):
    expected_output = [
        30.0, 36.0, 42.0, 66.0, 81.0, 96.0, 102.0, 126.0, 150.0, 138.0, 171.0,
        204.0, 174.0, 216.0, 258.0, 210.0, 261.0, 312.0
    ]
    self._VerifyValues(
        input_sizes=[1, 2, 3, 3],
        filter_sizes=[1, 1, 3, 3],
        stride=1,
        padding="VALID",
        expected=expected_output)



if __name__ == "__main__":
  googletest.main()
