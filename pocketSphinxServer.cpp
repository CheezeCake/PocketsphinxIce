#include <iostream>
#include <cstdio>
#include <Ice/Ice.h>
#include <pocketsphinx.h>
#include "pocketSphinx.h"

class PocketSphinxServer : public PocketSphinxIce::IPocketSphinxServer
{
	public:
		PocketSphinxServer();
		std::string decode(const PocketSphinxIce::sample& signal, const Ice::Current& c) override;

	private:
		ps_decoder_t* ps;
};

PocketSphinxServer::PocketSphinxServer()
{
	cmd_ln_t* cfg = cmd_ln_init(nullptr, ps_args(), TRUE,
			"-hmm", MODELDIR"/hmm/lium_french_f0",
			"-lm", MODELDIR"/lm/french3g62K.lm.dmp",
			"-dict", MODELDIR"/lm/frenchWords62K.dic",
			nullptr);
	if (!cfg)
		throw std::runtime_error("Error in cmd_ln_init");

	ps = ps_init(cfg);
	if (!ps)
		throw std::runtime_error("Error in ps_init");
}

std::string PocketSphinxServer::decode(const PocketSphinxIce::sample& signal, const Ice::Current& c)
{
	std::cout << "Decode\n";
	int rv;
	const char* hyp;
	const char* uttid;
	int32 score;

	rv = ps_start_utt(ps, NULL);
	if (rv < 0)
		throw PocketSphinxIce::Error("Error in ps_start_utt");

	ps_process_raw(ps, signal.data(), signal.size(), FALSE, FALSE);

	rv = ps_end_utt(ps);
	if (rv < 0)
		throw PocketSphinxIce::Error("Error in ps_end_utt");

	hyp = ps_get_hyp(ps, &score, &uttid);
	if (!hyp)
		throw PocketSphinxIce::Error("ps_get_hyp returned NULL");

	std::cout << "return:" << hyp << '\n';
	return hyp;
}

int main(int argc, char **argv)
{
	Ice::CommunicatorPtr ic;
	int status = 0;
	std::string port("20000");

	if (argc > 1) {
		try {
			std::stoul(argv[1]);
		}
		catch (const std::exception& e) {
			std::cerr << "Invalid port number: " << argv[1] << '\n';
			return 1;
		}

		port = argv[1];
	}

	try {
		ic = Ice::initialize(argc, argv);
		Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("PocketSphinxServerAdapter", "default -p " + port);
		PocketSphinxServer* srv = new PocketSphinxServer;
		std::cout << "init done\n";
		Ice::ObjectPtr object = srv;
		adapter->add(object, ic->stringToIdentity("PocketSphinxServer"));
		adapter->activate();
		ic->waitForShutdown();
	}
	catch (const Ice::Exception& e) {
		std::cerr << e << std::endl;
		status = 1;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		status = 1;
	}
	catch (...) {
		status = 1;
	}

	if (ic) {
		try {
			ic->destroy();
		}
		catch (const Ice::Exception& e) {
			std::cerr << e << std::endl;
			status = 1;
		}
	}

	return status;
}
