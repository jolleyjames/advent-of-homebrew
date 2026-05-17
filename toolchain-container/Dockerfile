# Build container

FROM devkitpro/devkitppc:latest

# install Eigen 5.0.0
WORKDIR /work 
RUN <<EOF
wget https://gitlab.com/libeigen/eigen/-/archive/5.0.0/eigen-5.0.0.tar.gz
tar -xzf eigen-5.0.0.tar.gz
mkdir eigen-build
cd eigen-build
cmake ../eigen-5.0.0
sudo make install
EOF

# delete working directory
WORKDIR /
RUN sudo rm -rf /work

# copy build script and run
COPY build.sh .
RUN chmod u+x ./build.sh
CMD [ "./build.sh", "/project" ]
