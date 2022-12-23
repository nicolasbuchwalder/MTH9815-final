#!/usr/bin/env bash

# list of files to run in correct order
files=(\
    "./dataproviders/bondinquiryprovider/bondinquiryprovider" \
    "./dataproviders/bondmarketdataprovider/bondmarketdataprovider" \
    "./dataproviders/bondtradebookingprovider/bondtradebookingprovider" \
    "./dataproviders/bondpricingprovider/bondpricingprovider" \
    "./tradingsystem/tradingsystem" \
    "./tradinglogs/bondexecutionlog/bondexecutionlog" \
    "./tradinglogs/bondstreaminglog/bondstreaminglog" \
    )

# runs make to compile if necessary
function makefiles() {
    echo "running make files to compile project if necessary"
    make
}

# macos new terminal and run script in 1st argument
function macosrunfile() {

    osascript \
    -e "on run(argv)" \
    -e "tell application \"Terminal\" to activate" \
    -e "tell application \"System Events\" to keystroke \"t\" using {command down}" \
    -e "tell application \"Terminal\" to do script item 1 of argv in front window" \
    -e "end run " \
    -- "$1"
}

# linux new terminal and run script in 1st argument
function linuxrunfile() {
    x-terminal-emulator -e "$1"
}

# function to run in mac
function macoslaunch() {
    for ((i = 0; i < ${#files[@]}; i++)); do
        macosrunfile "${files[$i]}"
        sleep 0.1
    done
}

# function to run in linux
function linuxlaunch() {
    for ((i = 0; i < ${#files[@]}; i++)); do
        linuxrunfile "${files[$i]}"
        sleep 0.1
    done
}

# main function
function launch() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "detected macos"
        
        makefiles
        if [ $? -eq 0 ]; then
            echo "compile successful"
            echo "launching all processes"
            macoslaunch
        else
            echo "compile unsuccessful, please check errors"
        fi

    else
        echo "detected linux"
        makefiles
        if [ $? -eq 0 ]; then
            echo "compile successful"
            echo "launching all processes"
            macoslaunch
        else
            echo "compile unsuccessful, please check errors"
        fi
    fi
}

function delete_execs(){
    for ((i = 0; i < ${#files[@]}; i++)); do
        if [ -f "${files[$i]}" ] ; then
            rm "${files[$i]}"
        fi
    done
    
}
function help() {
    echo "commands:"
    echo "    -r,--recompile    delete executables (if they exist) and recompile before launching"
    echo "    -d,--deleteafter  delete the executables after running"
    echo "    -b,--both         do both"
    echo "    -h,--help         gives help"
}

 
case $1 in
--recompile|-r)
delete_execs
launch
;;
--deleteafter|-d)
launch
delete_execs
;;
--both|-b)
delete_execs
launch
delete_execs
;;
--help|-h)
help
;;
*)
launch
;;
esac
exit 0



