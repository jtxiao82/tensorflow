from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

import tensorflow as tf
from tensorflow.python.ops import math_ops
from tensorflow.python.platform import test


class ZeroOutTest(tf.test.TestCase):
  def testZeroOut(self):
    with self.test_session():
      result = tf.user_ops.zero_out([5, 4, 3, 2, 1])
      self.assertAllEqual(result.eval(), [5, 0, 0, 0, 0])


if __name__ == "__main__":
  test.main()
