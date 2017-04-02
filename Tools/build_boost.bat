setlocal
cd %BOOST_ROOT%
b2 -j 8 link=static runtime-link=static threading=multi address-model=64
endlocal