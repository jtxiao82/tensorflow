bazel build --config=opt //tensorflow/tools/pip_package:build_pip_package # Install Tensorflow CPU
bazel build --config=opt --config=cuda //tensorflow/tools/pip_package:build_pip_package # Install Tensorflow GPU
bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg # Generate Tensorflow python package
sudo pip uninstall /tmp/tensorflow_pkg/tensorflow-1.3.0rc1-cp27-cp27mu-linux_x86_64.whl # Remove origin Tensorflow python package
sudo pip install /tmp/tensorflow_pkg/tensorflow-1.3.0rc1-cp27-cp27mu-linux_x86_64.whl  # Install new Tensorflow python package

