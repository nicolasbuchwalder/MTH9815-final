#!/usr/bin/env bash

files=(\
    "./dataproviders/bondinquiryprovider/bondinquiryprovider" \
    #"./dataproviders/bondmarketdataprovider/bondmarketdataprovider" \
    "./dataproviders/bondtradebookingprovider/bondtradebookingprovider" \
    "./dataproviders/bondpricingprovider/bondpricingprovider" \
    "./tradingsystem/tradingsystem" \
    "./tradinglogs/bondexecutionlog/bondexecutionlog" \
    "./tradinglogs/bondstreaminglog/bondstreaminglog" \
    )

function makefiles() {
    echo "running make files to compile project if necessary"
    make
}

# macos new terminal and run
function macosrunfile() {

    osascript \
    -e "on run(argv)" \
    -e "tell application \"Terminal\" to activate" \
    -e "tell application \"System Events\" to keystroke \"t\" using {command down}" \
    -e "tell application \"Terminal\" to do script item 1 of argv in front window" \
    -e "end run " \
    -- "$1"
}


# linux new terminal and run
function linuxrunfile() {
    x-terminal-emulator -e "$1"
}

function macoslaunch() {
    for ((i = 0; i < ${#files[@]}; i++)); do
	echo "${file}"
        macosrunfile "${files[$i]}"
        sleep 0.1
    done
}

function linuxlaunch() {
    for exec in ${files[@]}; do
        linuxrunfile ${exec}
    done
}


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

 



#$macosrun ./dataproviders/bondinquiryprovider/bondinquiryprovider
#${runscript} #./tradingsystem/tradingsystem
#(cd dataproviders/bondinquiryprovider; ./bondinquiryprovider) &
#(cd dataproviders/bondmarketdataprovider; ./bondmarketdataprovider) &
#(cd dataproviders/bondtradebookingprovider; ./bondtradebookingprovider) &
#(cd dataproviders/bondpricingprovider; ./bondpricingprovider) &
#(cd tradinglogs/bondinquiryprovider; ./bondinquiryprovider) &
#(cd tradingsystem; ./tradingsystem) &
#fg



