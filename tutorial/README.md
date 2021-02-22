# Hotrod Client dev tutorial

This project contains some example on how to setup an environment for developing hotrod applications.



### Use centos OCI image
Tutorials can run in a sandbox using the containerized image described by this [Dockerfile](https://github.com/infinispan/cpp-client/blob/master/prebuilt-tests/centos8/Dockerfile).

Step for this are:
- `cd $PROJECT_HOME/prebuilt-tests/centos8 && ./build-docker.sh`
- start an Infinispan server on localhost:11222, it will be accessible by the container since it will be started with `--network=host` option
- `podman run -v $PROJECT_HOME/cpp-client/tutorial:/home/jboss/tutorial:rw --network=host -it cpp-test-machine-c8:latest`

From inside the container:
- `cd tutorial`
- `wget https://ci.infinispan.org/job/Infinispan%20C++%20Client/job/master/lastSuccessfulBuild/artifact/build/infinispan-hotrod-cpp-8.1.0.SNAPSHOT-RHEL-x86_64.rpm`
- `rpm --force -i infinispan-hotrod-cpp-8.1.0.SNAPSHOT-RHEL-x86_64.rpm`
- `mkdir build && cd build && cmake .. && make`
- `./clientTutorial-1`


### Non containerized build
Tutorials need cmake and the hotrod libraries installed. If everything is in place tutorials should build and run this way:
- `mkdir build && cd build && cmake .. && make`
- start an Infinispan server on localhost:11222
- `./clientTutorial-1`
