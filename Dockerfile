FROM rust:1.33.0-slim

#11.04
#apt-install
RUN apt-get update && \
	apt-get upgrade -y && \
	apt-get install make && \
	apt-get install g++ build-essential libssl-dev pkg-config -y



# Copy source code of the cds server into the build container
ADD ./cds_server /tmp/cds_server
ADD ./mopp-2018-t0-harmonic-progression-sum /tmp/mopp-2018-t0-harmonic-progression-sum
ADD ./mopp-2018-t1-average /tmp/mopp-2018-t1-average
ADD ./mopp-2018-t2-levenshtein /tmp/mopp-2018-t2-levenshtein


#19.05
RUN apt-get install golang -y

RUN export GOPATH=/tmp && \
	export PATH=$PATH:$GOPATH

#19.05
RUN apt-get install golang -y

RUN export GOPATH=/tmp/mopp-2018-t2-levenshtein
RUN export PATH=$PATH:$GOPATH



# Build code and copy into container system paths
RUN cd /tmp/cds_server && \
    # build it
    cargo build --release && \
    # copy the built binary into the system
    cp target/release/cds_server /usr/local/bin && \
    # copy the server's config file into config directory
    cp cds_server.json /etc/cds_server.json && \
    # remove the source code from the build container
    rm -r /tmp/cds_server



#12.04
#compile ccp

RUN	make -C /tmp/mopp-2018-t0-harmonic-progression-sum
#	./harmonic-progression-sum
RUN make -C /tmp/mopp-2018-t1-average/
RUN make -C /tmp/mopp-2018-t2-levenshtein/

# Set default port the server will listen on
ENV CDS_PORT 8080

# Activate debug output by default to help debugging issues
ENV RUST_LOG debug

# Start the cds server by default with its configuration file read
# from /etc/cds_server.json and its port read from environment
# variable CDS_PORT


CMD cds_server -c /etc/cds_server.json -p $CDS_PORT
