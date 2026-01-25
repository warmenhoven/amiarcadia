package com.amigan.droidarcadia;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okhttp3.MediaType;
import java.io.IOException;

public class NetworkUtil
{   private static final OkHttpClient client = new OkHttpClient();

    public static native void chttpcallback(int statuscode, String content, long userdata, String errormessage);
    
    public static void asynchttppost(String urlstr, String postdatastr, String useragentstr, long asyncdata)
    {   RequestBody body    = RequestBody.create(postdatastr, MediaType.get("application/x-www-form-urlencoded"));
        Request     request = new Request.Builder()
                                         .url(urlstr)
                                         .post(body)
                                         .header("User-Agent", useragentstr)
                                         .build();

        client.newCall(request).enqueue(new Callback()
        {   @Override
            public void onFailure(Call call, IOException e)
            {   // on the first run we used to often get "unable to resolve host retroachievements.org"
                chttpcallback(-1, "", asyncdata, "Failed: " + e.getMessage() + "!"); // No content or size to pass on failure
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException
            {   int    statusCode   = response.code();          // eg. 200
                String responseBody = response.body().string(); // eg. {"Success",true,"User:","Minuous","AvatarUrl:","ht..."
                chttpcallback(statusCode, responseBody, asyncdata, "");
                response.body().close(); // must be done *after* httpcallback()!
            }
        });
}   }
