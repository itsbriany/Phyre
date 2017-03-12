setlocal
cd %BOOST_ROOT%
b2 -j 8 link=static threading=multi runtime-link=static address-model=64
endlocal