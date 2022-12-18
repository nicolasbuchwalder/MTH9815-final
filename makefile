all:
	@echo "compiling bondinquiryprovider";
	@cd dataproviders && cd bondinquiryprovider && $(MAKE) && cd ..;
	@echo "compiling bondmarketdataprovider";
	@cd dataproviders && cd bondmarketdataprovider && $(MAKE) && cd ..;
	@echo "compiling bondpricingprovider";
	@cd dataproviders && cd bondpricingprovider && $(MAKE) && cd ..;
	@echo "compiling bondtradebookingprovider";
	@cd dataproviders && cd bondtradebookingprovider && $(MAKE) && cd ..;
	@echo "compiling bondexecutionlog";
	@cd tradinglogs && cd bondexecutionlog && $(MAKE) && cd ..;
	@echo "compiling bondstreaminglog";
	@cd tradinglogs && cd bondstreaminglog && $(MAKE) && cd ..;
	@echo "compiling tradingsystem";
	@cd tradingsystem && $(MAKE);
	

