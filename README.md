CDS Lab 2019

Useful Links

Evaluation System Logs: https://cds-lab-2019.netlify.com/logs/
Give us git repository access:
GitHub:
Use deploy keys feature with GitHub: https://developer.github.com/v3/guides/managing-deploy-keys/#deploy-keys
Apparently, Github deploy keys can not be used as anticipated.
Instead we created a new Github user for our evaluation system that you can add as a collaborator to grant access: https://github.com/tudinfse-cds-evaluator.
Use access keys feature with Bitbucket: https://confluence.atlassian.com/bitbucket/access-keys-294486051.html#Accesskeys-Step3.Addthepublickeytoyourrepository
Docker Introduction:
Using images: https://docker-curriculum.com/ (skip the AWS and Docker Hub related parts - they're not necessary for the lab)
Creating images: https://www.linux.com/blog/learn/intro-to-linux/2018/1/how-create-docker-image
More information: https://docs.docker.com/
CDS Tools:
Compiling Rust: https://www.rust-lang.org/learn/get-started
CDS Server: https://github.com/tudinfse/cds_server
CDS Evaluator: https://github.com/tudinfse/cds_evaluator
Task 1: Prerequirement & Registration

This task will set up your development environment and ensure you're able to interact with our evaluation system.

To complete this task you have to fulfill the following requirements:

Create a private git repository where you will hold your source code.

We will check that your repository is private (access is not granted without authentication).

Grant read access to your repository to our the cds evaluation system. The public key the evaluation system uses:

ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIBFbAzSy3BevnEp23HVQMQONPFAj7GR6zcwDvcz/c5Jb
In the useful links section you'll find links on how to provide access to a key to an individual repository on GitHub and Bitbucket.

Our evaluation system will use your master branch.

Sent us your repository url (the one with which one can clone the repository) via email or via the repository registration form. We will add you to the evaluation system and sent you the link where you'll find the evaluation system results.

The Dockerfile has to be placed in the root directory of your repository.

The evaluation system will build your repository using Docker. Therefore your repository has to contain a Dockerfile. You can use the following as a start for your Dockerfile:

FROM tudinfse/cds_server
With this Dockerfile your image will be based on our cds_server image that already contains the necessary server (see next step).

You should make sure the following commands work for your repository:

$ git clone REPO_URL repo
$ cd repo
$ git checkout origin/master
$ docker build -t cds-lab-image .
The image build from your Dockerfile has to expose the cds_server interface.

The evaluation system will measure the time it takes to execute certain programs. For this is it has to be able to invoke the programs from outside the running container. We have a minimalistic webserver for this that you can use to expose the required interface. More information about the cds_server can be obtained via the link below.

Make sure the image build from you repository exposes the necessary interface. You can achieve this, for example, by using our tudinfse/cds_server image as a base image for your image in your Dockerfile as shown above. Or you could copy the content of our cds_server git repository into your repository and use the Dockerfile provided within to compile the cds_server and copy it and its configuration file to the correct location.

You can check that your repository meets this requirement by ensuring the following commands work for it. Note that, 1) cds-lab-image is the name of the image created in the previous step and 2) the echo program has to be configured for the following commands to work (our tudinfse/cds_server is configured for this).

$ # Start the image in background
$ # '-eCDS_PORT=7227' tells the cds_server within the container to listen on port 7227.
$ # Without this the evaluation tool will not be able to communicate with the server.
$ # See the Dockerfile in the cds_server repository to understand how this works.
$ # '-p7227:7227' instructs docker to forward port 7227 from the inside of the container to port 7227 on the host.
$ docker run -d -eCDS_PORT=7227 -p7227:7227 cds-lab-image
ccf1d4dabaaee09a804a2e3ee086b1b1189f0c10f2e08911e443f4021f1eb189
$ # Query the container's server
$ curl localhost:7227/run/echo -XPOST --data "{\"stdin\":\"$(echo "it works!"|base64)\"}"
{"stdout":"aXQgd29ya3MhCg==","stderr":"","exit_status":0,"duration":979,"error":null}
$ # Stop and remove container
$ docker rm --force ccf1d4dabaaee
ccf1d4dabaaee09a804a2e3ee086b1b1189f0c10f2e08911e443f4021f1eb189
Also note that, stdin, stdout, and stderr are base64-encoded. You can use command line tools to decode the program's stdout:

$ echo "aXQgd29ya3MhCg==" | base64 -d
it works!
The cds_server shall offer access to a parallelized solution of the "Harmonic Progression Sum" problem from MoPP 2018.

We offer a parallized source code on the course's website. Note that, this source code contains a bug that prevents it from exhibiting parallelism. You have to find the bug and fix it for the program to be able to execute in parallel!

To add the program to your image you have to edit your Dockerfile to

copy in the program's source code,
install compilation dependencies via the container's package manager (apt install in the case of our tudinfse/cds_server image),
compile the program using make, and
edit (or replace) the cds_server configuration file to include an entry for the newly compiled program (look into the cds_server repository for details about the configuration file format).
Note that, we refer to this program using the string mopp-2018-t0-harmonic-progression-sum. Thus, make sure that you use exactly this name, otherwise the evaluation system will not be able to pick up your program.

You can use the cds_evaluator repository (find the link below) to check for correct functioning of you image:

$ # In cds_evaluator repository
$ # Build the evaluator (already compiled in this case)
$ cargo build --release
Finished release [optimized] target(s) in 0.21s
$ # Evaluate the task using cds_evaluator
$ target/release/cds_evaluator -m --input tasks/ mopp-2018-t0-harmonic-progression-sum/judge.in --output tasks/  mopp-2018-t0-harmonic-progression-sum/judge.out --image cds-lab-image mopp-2018-t0-harmonic-progression-sum
program; run; cpus; duration;
harmonic; 0; 1; 100937021
harmonic; 0; 2; 100936000
harmonic; 0; 4; 101813369
harmonic; 1; 1; 102968769
...
You can also use the curl command, as before, to check your image:

$ # In cds_evaluator repository
$ docker run -d -eCDS_PORT=7227 -p7227:7227 cds-lab-image
ccf1d4dabaaee09a804a2e3ee086b1b1189f0c10f2e08911e443f4021f1eb189
$ curl localhost:7227/run/mopp-2018-t0-harmonic-progression-sum -XPOST --data "  {\"stdin\":\"$(cat tasks/mopp-2018-t0-harmonic-progression-sum/judge.in|base64)  \"}"
{"stdout":"Mi41OTI4NTcxNDI4NTcK","stderr":"UnVubmluZyBvbiA0IENQVXMKU3RhcnRpbmcgdGhyZWFkIDAgZnJvbSAxIHRvIDcK","exit_status":0,"duration":1363,"error":null}
$ docker rm --force ccf1d4dabaaee0
Sent us an email after everything works .

In this email you should include

The local measurement results (of the bugfree version), and
a link to your results on our evaluation server.
We will check your results to see if you have everything set up correctly and obtain proper speedup for the harmonic progression sum problem.

Students solving this task will be assigned two programs they have to optimize and parallelize throughout the semester.

Development Environment

We assume that you have a Ubuntu based system. Of course, you can also use different systems but we generally do not offer support for different OS.

Linux (Ubuntu)

Install dependencies:

You need git and ssh to interact with you repository, docker to build and run docker images and some dependencies to build the evaluator:

$ sudo apt update -y
$ sudo apt install -y git ssh docker.io build-essential libssl-dev pkg-config
Install rust:

Rust is easily installed via rustup:

$ curl https://sh.rustup.rs -sSf | sh
Follow the instructions on screen. You can stick with the default options. Further information can be found here: https://www.rust-lang.org/learn/get-started

Check that everything works as expected using the evaluator and our tudinfse/cds_server image:

$ git clone https://github.com/tudinfse/cds_evaluator.git
Cloning into 'cds_evaluator'...
remote: Enumerating objects: 44, done.
remote: Counting objects: 100% (44/44), done.
remote: Compressing objects: 100% (24/24), done.
remote: Total 44 (delta 15), reused 44 (delta 15), pack-reused 0
Unpacking objects: 100% (44/44), done.
$ cd cds_evaluator/
$ cargo build --release
    [...]
    Finished release [optimized] target(s) in 2m 22s
$ ./target/release/cds_evaluator -m echo -i tasks/echo/judge.in -o tasks/echo/judge.out --image tudinfse/cds_server
program; run; cpus; duration;
echo; 0; 1; 321
echo; 0; 2; 263
echo; 0; 4; 261
echo; 1; 1; 352
echo; 1; 2; 374
echo; 1; 4; 310
echo; 2; 1; 317
echo; 2; 2; 241
echo; 2; 4; 243
$ # If you system only fewer than four cores you can limit the core usage range with the `-c` option. For example, using up to two cores:
$ ./target/release/cds_evaluator -m echo -i tasks/echo/judge.in -o tasks/echo/judge.out --image tudinfse/cds_server -c 1,2
program; run; cpus; duration;
echo; 0; 1; 391
echo; 0; 2; 389
echo; 1; 1; 323
echo; 1; 2; 292
echo; 2; 1; 375
echo; 2; 2; 275
Windows 10

Download and install Docker for Windows:
Download and execute https://download.docker.com/win/stable/Docker for Windows Installer.exe
During installation, do NOT select 'Use Windows containers instead of Linux containers'!
You might have to logout and login again after installation.
Start Docker Desktop. You might have to enable the Hyper-V and Containers features and restart after the first start.
Modify Docker Desktop Settings
Open Settings: right-click on the symbol in the system tray -> Settings
Activate "Expose daemon on tcp://localhost:2375 without TLS" under "General"
Increase CPU count under "Advanced" - the more the better
Install and setup Windows Subsystem for Linux (WSL)
Follow this guide: https://docs.microsoft.com/en-us/windows/wsl/install-win10 to install Ubuntu in WSL
Once you created your user account execute the following command to make WSL use the previously installed Docker Desktop:
$ echo "export DOCKER_HOST=tcp://localhost:2375" >> ~/.bashrc && source ~/.bashrc
Follow above Linux instruction from here