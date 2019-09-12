//- Water infiltration event
bool infiltrationEventIsPresent = transportProperties.found("eventFileInfiltration");
word infiltrationEventFileName = transportProperties.lookupOrDefault<word>("eventFileInfiltration","");
infiltrationEventFile infiltrationEvent(infiltrationEventFileName);
if (infiltrationEventIsPresent)
{
    infiltrationEvent.update(runTime.startTime().value());
    infiltrationEvent.storeOldValues();
}

//- Tracer injection event
bool sourceEventIsPresent = transportProperties.found("eventFileSource");
word sourceEventFileName = transportProperties.lookupOrDefault<word>("eventFileSource","");
sourceEventFile sourceEvent(sourceEventFileName);
if (sourceEventIsPresent)
{
    sourceEvent.update(runTime.startTime().value());
    sourceEvent.storeOldValues();
}

bool outputEventIsPresent = runTime.controlDict().found("eventFileOutput");
word outputEventFileName = runTime.controlDict().lookupOrDefault<word>("eventFileOutput","");
outputEventFile outputEvent(outputEventFileName);
if (outputEventIsPresent) outputEvent.update(runTime.startTime().value());

scalar eventTimeStep(runTime.controlDict().lookupOrDefault<scalar>("eventTimeStep",0));
if (eventTimeStep > 0)
{
    if (infiltrationEventIsPresent) infiltrationEvent.addIntermediateTimeSteps(eventTimeStep);
}

//- Updating infiltration value using event file
if (infiltrationEventIsPresent)
{
    if (infiltrationEvent.isUniform())
    {
        infiltration.primitiveFieldRef() = infiltrationEvent.currentValue(0);
    }
    else
    {
        if (infiltrationEvent.currentValues().size() != mesh.C().size())
        {
            Warning() << "Number of infiltration values does not correspond to the mesh size, first value is used as uniform infiltration" << endl;
            infiltrationEvent.setInfiltrationAsUniform();
            infiltration.primitiveFieldRef() = infiltrationEvent.currentValue(0);
        }
        else
        {
            infiltration.primitiveFieldRef() = infiltrationEvent.currentValues();
        }

    }
}

//- determining cell id corresponding tracer injection points
labelList idCoordinates(sourceEvent.ncoordinates());
if (sourceEventIsPresent)
{
    sourceTerm.primitiveFieldRef() = 0;
    forAll(sourceEvent.coordinates(),pointi)
    {
        idCoordinates[pointi] = mesh.findNearestCell(sourceEvent.coordinates()[pointi]);
        sourceTerm[idCoordinates[pointi]] += sourceEvent.currentValue(pointi)/mesh.V()[idCoordinates[pointi]];
    }
}