# tpush

a tiny push server for lots of connections message notify.

# build and install

tpush is depended on tnet(a tiny server for high performance network programming).

## checkout tnet as submodule. 

    git submodule init
    git submodule update

## create a build folder for building tpush

    mkdir -p build
    cd build
    
    cmake ..
    make

If you want to use tcmalloc and build release version, do below

    cmake .. -DCMAKE_USE_TCMALLOC=1 -DCMAKE_BUILD_TYPE=Release 


# issue

Some issue I meet, maybe you may meet too!

- Pushing to Git returning Error Code 403 fatal: HTTP request failed

I use below to solve:
    
        git remote set-url origin https://username@github.com/username/tpush.git

Then I try push again because network is poor sometimes;
    
- git submodule 
    
refer [this](http://www.kafeitu.me/git/2012/03/27/git-submodule.html) 
