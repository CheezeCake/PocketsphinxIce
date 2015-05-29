module PocketSphinxIce
{
	exception Error
	{
		string what;
	};

	sequence<short> sample;

	interface IPocketSphinxServer
	{
		string decode(sample signal) throws Error;
	};
};
