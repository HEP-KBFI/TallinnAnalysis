#!/usr/bin/env python3

import functools
import json
import os
import matplotlib.pyplot as plt
plt.style.use('seaborn-whitegrid')

inputFilePath1 = "/home/veelken/newTallinnAnalysis/CMSSW_12_3_1/src/TallinnAnalysis/Framework/test/DEBUG_newSoftware/"
jsonFileName1 = "datacard_HH_2lss_nonresonant_oldSoftware_fullSample.json"
label1 = "old Software"

inputFilePath2 = "/home/veelken/newTallinnAnalysis/CMSSW_12_3_1/src/TallinnAnalysis/Framework/test/DEBUG_newSoftware/"
jsonFileName2 = "datacard_HH_2lss_nonresonant_newSoftware_fullSample.json"
label2 = "new Software"

# load dictionaries from JSON files
jsonFile1 = open(os.path.join(inputFilePath1, jsonFileName1))
dict1 = json.load(jsonFile1)
jsonFile1.close()
print("Number of events contained in file '%s': %i" % (jsonFileName1, len(dict1.keys())))

jsonFile2 = open(os.path.join(inputFilePath2, jsonFileName2))
dict2 = json.load(jsonFile2)
jsonFile2.close()
print("Number of events contained in file '%s': %i" % (jsonFileName2, len(dict2.keys())))

# determine common events that are contained in both JSON files
events1 = set(dict1.keys())
events2 = set(dict2.keys())
common_events = list(events1.intersection(events2))
print("Number of common events: %i" % len(common_events))
if len(common_events) == 0:
    raise ValueError("Found no common events --> aborting !!")

# determine which variables to plot
variables = list(dict1[common_events[0]].keys())

# make scatter plots of variables in file1 (x-axis) vs variables in file2 (y-axis) 
for variable in variables:
    if variable in [ "SM" ] or variable.find("JHEP04") != -1:
        print("Skipping variable = %s" % variable)
        continue
    print("Making correlation plot for variable = %s" % variable)
    x = []
    y = []
    idxEvent = 0
    for event in common_events:
        value1 = dict1[event][variable]
        x.append(value1)
        value2 = dict2[event][variable]
        y.append(value2)
        ##print("event #%i: x = %1.3f, y = %1.3f" % (idxEvent, value1, value2))
        idxEvent += 1
    plt.clf()
    plt.scatter(x, y, marker = 'o', s = 5)
    plt.xlabel("%s in %s" % (variable, label1))
    plt.ylabel("%s in %s" % (variable, label2))
    outputFileName = "compareJSONs_%s.png" % variable
    plt.savefig(outputFileName)

print(" Done.")
