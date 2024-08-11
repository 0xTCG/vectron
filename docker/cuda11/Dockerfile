FROM nvidia/cuda:11.6.1-devel-ubuntu20.04

WORKDIR /root

RUN apt update && apt install -y git curl time

RUN curl -fsSL https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS-2023.PUB | gpg --dearmor | tee /usr/share/keyrings/intel-oneapi-archive-keyring.gpg && \
echo "deb [signed-by=/usr/share/keyrings/intel-oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main " > /etc/apt/sources.list.d/oneAPI.list

RUN apt-get update && apt-get install -y intel-oneapi-compiler-dpcpp-cpp-and-cpp-classic-2023.2.0

RUN git clone https://github.com/vtsynergy/aalign /aalign && \
	cd /aalign/ModularDesign && \
	export PATH=/opt/intel/oneapi/compiler/2023.2.0/linux/bin/intel64:${PATH} && \
	make -j APPNAME=SmithWatermanAffineGapMod && \
	make -j APPNAME=SmithWatermanMod && \
	make -j APPNAME=NeedlemanWunschAffineGapMod && \
	make -j APPNAME=NeedlemanWunschMod

RUN git clone https://github.com/mkorpar/swsharp /swsharp

RUN cd /swsharp && sed -i 's/_30/_50/g' swsharp/Makefile && make -j

COPY seqx.fasta /others/data/

COPY seqy.fasta /others/data/

CMD ["bash"]
