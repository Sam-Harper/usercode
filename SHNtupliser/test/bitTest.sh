#!/usr/bin/env bash

datasetType=$1
datasetNr=$2


datasetCode=$(($datasetType<<12))
datasetCode=$(($datsetCode & $(($datasetNr))))

echo "datasetCode $datasetCode"

exit
