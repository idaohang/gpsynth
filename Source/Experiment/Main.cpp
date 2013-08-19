/*
  ==============================================================================

    Main.cpp
    Author:  cdonahue

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "GPExperiment.h"
#include "boost/program_options.hpp"

int main( int argc, const char* argv[] )
{
    namespace po = boost::program_options;

    // temp fields
    std::string target;
    std::string output_file_dir;
    unsigned seed;
    std::vector<float> constants;
    bool print_experiment_info;

    // param structs
    GPMatchingExperimentParams* me_params = (GPMatchingExperimentParams*) malloc(sizeof(GPMatchingExperimentParams));

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "display help message")
        
        ("output_dir", po::value<std::string>(&output_file_dir)->default_value("./"), "output file directory")
        ("exp_info", po::value<bool>(&print_experiment_info)->default_value(false), "print info about experiment environment")
        ("target", po::value<std::string>(&target), "target wav file")
        ("constants", po::value<std::vector<float> >(&constants), "special constant values that synthesis algorithms can use")
        ("seed", po::value<unsigned>(&seed)->default_value(time(NULL)), "experiment seed number")

        ("log_save_precision", po::value<unsigned>(&(me_params->log_save_precision))->default_value(20), "precision for saving/backing up synthesis algorithms")
        ("log_print_precision", po::value<unsigned>(&(me_params->log_print_precision))->default_value(3), "precision for text-logging synthesis algorithms")
        ("log_save_gen_champ_audio", po::value<bool>(&(me_params->log_save_gen_champ_audio))->default_value(false), "save rendered audio of the champion from each generation")
        ("log_save_overall_champ_audio", po::value<bool>(&(me_params->log_save_overall_champ_audio))->default_value(false), "save rendered audio of the champion from the entire run")
        ("log_save_target_spectrum", po::value<bool>(&(me_params->log_save_target_spectrum))->default_value(false), "save spectrum of target wav file")
        ("log_save_target_copy", po::value<bool>(&(me_params->log_save_target_copy))->default_value(false), "save copy of target wav file")

        ("aux_wav_file_buffer_size", po::value<unsigned>(&(me_params->aux_wav_file_buffer_size))->default_value(512), "buffer size for wav file IO")
        ("aux_render_block_size", po::value<unsigned>(&(me_params->aux_render_block_size))->default_value(512), "chunk size for rendering synthesis algorithms")
 
        ("exp_number", po::value<unsigned>(&(me_params->exp_number))->default_value(1), "which experiment to run")
        ("exp_suboptimize_type", po::value<unsigned>(), "which type of suboptimization to perform")
        ("exp_generations", po::value<unsigned>(), "maximum number of generations to evaluate")
        ("exp_threshold", po::value<float>(), "fitness to reach to terminate evolution")

        ("ff_type", po::value<unsigned>(), "which fitness function to use")
        ("ff_spectrum_mag_weight", po::value<float>(), "weight multiplier for spectrum magnitudes in fitness function")
        ("ff_spectrum_phase_weight", po::value<float>(), "weight multiplier for spectrum phases in fitness function")
        
        ("ff_fft_window", po::value<std::string>(), "fft window type")
        ("ff_fft_size", po::value<unsigned>(), "fft size")
        ("ff_fft_overlap", po::value<unsigned>(), "fft overlap")

        ("ff_moving_average_type", po::value<unsigned>(), "type of moving average to use for fitness function comparison")
        ("ff_moving_average_past_radius", po::value<unsigned>(), "# of feedback samples for moving average filter")
        ("ff_moving_average_future_radius", po::value<unsigned>(), "# of feedforward samples for moving average filter")
        ("ff_moving_average_exponential_alpha", po::value<double>(), "importance of successive samples in the moving average")
        ("ff_weight_frames", po::value<bool>(), "weight importance of target frames in comparison")
        ("ff_weight_frames_exponent", po::value<double>(), "exponent for target frames weighting")

        ("ff_phase_comparison_exponent", po::value<double>(), "exponent for penalizing bad phase")
        ("ff_mag_good_comparison", po::value<double>(), "penalty bonus for comparing bins which are on the correct side of the moving average")
        ("ff_mag_bad_comparison", po::value<double>(), "penalty bonus for comparing bins which are on the incorrect side of the moving average")
        ("ff_mag_base_comparison", po::value<double>(), "penalty for comparing magnitudes of bins in each frame")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

}

void printImportantExperimentInfo() {
    // PRINT COMP INFORMATION TO ERROR LOG
    time_t now = time(0);
    struct tm tstruct;
    char buff[200];
    tstruct = *localtime(&now);
    strftime(buff, sizeof(buff), "Date run: %m.%d.%Y\nTime run: %H.%M.%S\n", &tstruct);

    // PRINT TIME/DATE
    std::cerr << buff;

    // PRINT GITHUB COMMIT ID
    FILE *gitid = popen("git rev-parse HEAD", "r");
    char gitbuffer[200];
    while (fgets(gitbuffer, sizeof(gitbuffer) - 1, gitid) != NULL) {
        std::cerr << "Git commit ID: " << gitbuffer;
    }
    pclose(gitid);

    // PRINT HOST INFO
    FILE *hostname = popen("hostname", "r");
    char hostbuffer[200];
    while (fgets(hostbuffer, sizeof(hostbuffer) - 1, hostname) != NULL) {
        std::cerr << "Host name: " << hostbuffer;
    }
    pclose(hostname);

    // PRINT CPU INFO
    FILE *lscpu = popen("lscpu", "r");
    char lscpubuffer[1024];
    while (fgets(lscpubuffer, sizeof(lscpubuffer) - 1, lscpu) != NULL) {
        std::cerr << lscpubuffer;
    }
    pclose(lscpu);

    // PRINT MEMORY INFO
    FILE *meminfo = popen("grep \"Mem\" /proc/meminfo", "r");
    char meminfobuffer[200];
    while (fgets(meminfobuffer, sizeof(meminfobuffer) - 1, meminfo) != NULL) {
        std::cerr << meminfobuffer;
    }
    pclose(meminfo);
}


    //==============================================================================
/*
void initialise (const String& commandLine)
    {
        Thread::setCurrentThreadName("experiment");

        // This method is where you should put your application's initialisation code..
        StringArray args = getCommandLineParameterArray();

        // init params
        params = (GPParams*) malloc(sizeof(GPParams));

		// synth and experiment shared params
        params->verbose = false;
        params->savePrecision = 4;
        params->printPrecision = 3;
        //params->lowerFitnessIsBetter = true; should be done in experiment
        //params->bestPossibleFitness = 0; should be done in experiment
		
        // experiment params
        params->experimentNumber = 0;
        params->suboptimizeType = 0;
        params->fitnessFunctionType = 1;
        params->numGenerations = 100;
        params->thresholdFitness = 0.0;
        params->ephemeralRandomConstants = true;

        // auxillary params
        params->saveGenerationChampions = false;
        params->saveTargetEnvelope = false;
        params->saveTargetSpectrum = false;
        params->backupTarget = false;
        params->wavFileBufferSize = 256;
        params->renderBlockSize = 256;

        // fitness function weights
        params->magnitudeWeight = 1.0;
        params->phaseWeight = 0.1;
        params->envelopeWeight = 0.5;

        // time domain fitness parameters
        params->envelopeFollowerAttack = 1;
        params->envelopeFollowerDecay = 300;
        params->compareEnvelopeNumSkipFrames = 15;

        // frequency domain fitness parametersi
        strncpy(params->windowType, "rect", 5);
        params->fftSize = 256;
        params->fftOverlap = 0;
        params->dBMagnitude = false;
        // moving average
        params->averageComparisonType = 1;
        params->movingAveragePastRadius = 50;
        params->movingAverageFutureRadius = 50;
        params->exponentialMovingAverageAlpha = 0.2;
        params->compareToMaxDeviation = true;
        params->penaltyComparisonExponent = 1.0;
        params->weightFftFrames = false;
        params->frameWeightExponent = 0.33;
        // phase penalty
        params->penalizeBadPhase = 2;
        // magnitude penalty
        params->goodComparisonFactor = 0.1;
        params->badComparisonFactor = 1.1;
        params->baseComparisonFactor = 0.9;

        // synth evolution params
        params->populationSize = 500;
        params->backupAllNetworks = false;
        params->backupPrecision = 100;
        params->maxInitialHeight = 5;
        params->maxHeight = 12;

        // synth genetic params
        params->proportionOfPopulationForGreedySelection = 0.0;
        // numeric mutation
        params->numericMutationSelectionType = 1;
        params->proportionOfPopulationFromNumericMutation = 0.05;
        params->percentileOfPopulationToSelectFromForNumericMutation = 0.05;
        params->numericMutationTemperatureConstant = 0.9;
        // mutation
        params->mutationSelectionType = 1;
        params->mutationType = 0;
        params->proportionOfPopulationFromMutation = 0.05;
        params->percentileOfPopulationToSelectFromForMutation = 0.05;
        // crossover
        params->crossoverSelectionType = 0;
        params->crossoverType = 0;
        params->proportionOfPopulationFromCrossover = 0.9;
        // reproduction
        params->reproductionSelectionType = 0;
        params->proportionOfPopulationFromReproduction = 0.05;

        // value node params
        params->valueNodeMinimum = -1.0;
        params->valueNodeMaximum = 1.0;

        // oscil node params
        params->oscilNodeMaxPartial = 30;
        params->oscilNodeMinIndexOfModulation = 0.0;
        params->oscilNodeMaxIndexOfModulation = 2.0;

        // modulation node
        params->LFONodeFrequencyRange = 10;
		
        // delay node
        params->delayNodeBufferMaxSeconds = 1.0;

        // filter node
        params->filterNodeCenterFrequencyMinimum = 20;
        params->filterNodeCenterFrequencyMaximumProportionOfNyquist = 0.9999;
        params->filterNodeQualityMinimum = 0.1;
        params->filterNodeQualityMaximum = 10.0;
        params->filterNodeBandwidthMinimum = 10;
        params->filterNodeBandwidthMaximum = 2000;

        // adsr node
        params->ADSRNodeEnvelopeMin = 0.0;
        params->ADSRNodeEnvelopeMax = 1.0;

        // parse command line
        for (String* i = args.begin(); i < args.end(); i++) {
            if (i->equalsIgnoreCase("--help")) {
                printHelp();
                return;
            }
            // temp fields
            if (i->equalsIgnoreCase("--target")) {
                target = *(++i);
            }
            else if (i->equalsIgnoreCase("--path")) {
                saveFilesTo = *(++i);
            }
            else if (i->equalsIgnoreCase("--seed")) {
                seed = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--values")) {
                String* current = (++i);
                while ( (i != args.end())  && !((current + 1)->startsWith(String("--"))) ) {
                    constants.push_back(current->getDoubleValue());
                    current = ++i;
                }
                constants.push_back(current->getDoubleValue());
            }
            else if (i->equalsIgnoreCase("--expinfo")) {
                printExperimentInfo = true;
            }
			
			// shared params
            else if (i->equalsIgnoreCase("--verbose")) {
                params->verbose = true;
            }
            else if (i->equalsIgnoreCase("--succinct")) {
                params->verbose = false;
            }
            else if (i->equalsIgnoreCase("--saveprecision")) {
                params->savePrecision = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--printprecision")) {
                params->printPrecision = (++i)->getIntValue();
            }

            // experiment params
            else if (i->equalsIgnoreCase("--experiment")) {
                params->experimentNumber = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--suboptimizetype")) {
                params->suboptimizeType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--fitnesstype")) {
                params->fitnessFunctionType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--generations"))  {
                params->numGenerations = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--threshold"))  {
                params->thresholdFitness = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--erc")) {
                params->ephemeralRandomConstants = true;
            }
            else if (i->equalsIgnoreCase("--noerc")) {
                params->ephemeralRandomConstants = false;
            }

            // auxillary params
            else if (i->equalsIgnoreCase("--savegenchamps")) {
                params->saveGenerationChampions = true;
            }
            else if (i->equalsIgnoreCase("--nosavegenchamps")) {
                params->saveGenerationChampions = false;
            }
            else if (i->equalsIgnoreCase("--savetargetenv")) {
                params->saveTargetEnvelope = true;
            }
            else if (i->equalsIgnoreCase("--nosavetargetenv")) {
                params->saveTargetEnvelope = false;
            }
            else if (i->equalsIgnoreCase("--savetargetspectrum")) {
                params->saveTargetSpectrum = true;
            }
            else if (i->equalsIgnoreCase("--nosavetargetspectrum")) {
                params->saveTargetSpectrum = false;
            }
            else if (i->equalsIgnoreCase("--backuptarget")) {
                params->backupTarget = true;
            }
            else if (i->equalsIgnoreCase("--nobackuptarget")) {
                params->backupTarget = false;
            }
            else if (i->equalsIgnoreCase("--loadwavblock")) {
                params->wavFileBufferSize = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--render")) {
                params->renderBlockSize = (++i)->getIntValue();
            }

            // fitness function weights
            else if (i->equalsIgnoreCase("--ffmagweight"))  {
                params->magnitudeWeight = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--ffphaseweight"))  {
                params->phaseWeight = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--ffenvweight"))  {
                params->envelopeWeight = (++i)->getDoubleValue();
            }

            // time domain fitness params
            else if (i->equalsIgnoreCase("--envatk"))  {
                params->envelopeFollowerAttack = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--envdcy"))  {
                params->envelopeFollowerDecay = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--envskip")) {
                params->compareEnvelopeNumSkipFrames = (++i)->getIntValue();
            }

            // frequency domain fitness params
            else if (i->equalsIgnoreCase("--window"))  {
                String* window = ++i;
                if (window->equalsIgnoreCase("rect")) {
                    strncpy(params->windowType, "rect", 5);
                }
                else if (window->equalsIgnoreCase("hann")) {
                    strncpy(params->windowType, "hann", 5);
                }
            }
            else if (i->equalsIgnoreCase("--fft")) {
                params->fftSize = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--fftoverlap")) {
                params->fftOverlap = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--dbmag")) {
                params->dBMagnitude = true;
            }
            else if (i->equalsIgnoreCase("--ampmag")) {
                params->dBMagnitude = false;
            }
            else if (i->equalsIgnoreCase("--averagetype")) {
                params->averageComparisonType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--averagepastradius")) {
                params->movingAveragePastRadius = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--averagefutureradius")) {
                params->movingAverageFutureRadius = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--alpha"))  {
                params->exponentialMovingAverageAlpha = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--comparedev")) {
                params->compareToMaxDeviation = true;
            }
            else if (i->equalsIgnoreCase("--comparerat")) {
                params->compareToMaxDeviation = false;
            }
            else if (i->equalsIgnoreCase("--penaltycompexp"))  {
                params->penaltyComparisonExponent = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--weightframes")) {
                params->weightFftFrames = true;
            }
            else if (i->equalsIgnoreCase("--noweightframes")) {
                params->weightFftFrames = false;
            }
            else if (i->equalsIgnoreCase("--frameweightexp"))  {
                params->frameWeightExponent = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--phasepenalty"))  {
                params->penalizeBadPhase = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--goodmagcomp"))  {
                params->goodComparisonFactor = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--badmagcomp"))  {
                params->badComparisonFactor = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--basemagcomp"))  {
                params->baseComparisonFactor = (++i)->getDoubleValue();
            }

            // synth evolution parameters
            else if (i->equalsIgnoreCase("--popsize")) {
                params->populationSize = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--backup")) {
                params->backupAllNetworks = true;
            }
            else if (i->equalsIgnoreCase("--nobackup")) {
                params->backupAllNetworks = false;
            }
            else if (i->equalsIgnoreCase("--backupprecision")) {
                params->backupPrecision = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--maxinitheight")) {
                params->maxInitialHeight = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--maxheight")) {
                params->maxHeight = (++i)->getIntValue();
            }

            // synth genetic params
            else if (i->equalsIgnoreCase("--greedy"))  {
                params->proportionOfPopulationForGreedySelection = (++i)->getDoubleValue();
            }
            // numeric mutation
            else if (i->equalsIgnoreCase("--nmselect")) {
                params->numericMutationSelectionType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--numericmutation")) {
                params->proportionOfPopulationFromNumericMutation = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--nmselectparam")) {
                params->percentileOfPopulationToSelectFromForNumericMutation = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--nmtemperature")) {
                params->numericMutationTemperatureConstant = (++i)->getDoubleValue();
            }
            // mutation
            else if (i->equalsIgnoreCase("--mselect")) {
                params->mutationSelectionType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--mtype")) {
                params->mutationType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--mutation")) {
                params->proportionOfPopulationFromMutation = (++i)->getDoubleValue();
            }
            else if (i->equalsIgnoreCase("--mselectparam")) {
                params->percentileOfPopulationToSelectFromForMutation = (++i)->getDoubleValue();
            }
            // crossover
            else if (i->equalsIgnoreCase("--cselect")) {
                params->crossoverSelectionType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--ctype")) {
                params->crossoverType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--crossover")) {
                params->proportionOfPopulationFromCrossover = (++i)->getDoubleValue();
            }
            // reproduction
            else if (i->equalsIgnoreCase("--rselect")) {
                params->reproductionSelectionType = (++i)->getIntValue();
            }
            else if (i->equalsIgnoreCase("--reproduction")) {
                params->proportionOfPopulationFromReproduction = (++i)->getDoubleValue();
            }
        }

        // TODO: check all value ranges here
        if (target.equalsIgnoreCase("") && params->experimentNumber != 0) {
            std::cerr << "No target specified. Exiting application." << std::endl;
            quit();
        }

        if (printExperimentInfo) {
            std::cerr << commandLine << std::endl;
            printImportantExperimentInfo();
        }

        // init rng
        rng = new GPRandom(seed);

        // start experiment
        requestedQuit = false;
        experiment = new GPExperiment(params, rng, seed, target, saveFilesTo, constants.size(), constants.data(), &requestedQuit);

        // TODO start a new thread to do this
        //juce::Thread experiment(String("evolution"));
        GPNetwork* champion = experiment->evolve();
        quit();
    }

*/
